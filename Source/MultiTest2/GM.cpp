// Fill out your copyright notice in the Description page of Project Settings.


#include "GM.h"

#include "EngineUtils.h"
#include "GI.h"
#include "GS.h"
#include "MyCharacter.h"
#include "PC.h"
#include "PS.h"
#include "Minion.h"
#include "Engine/PawnIterator.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "GameSystem/MatchTimer.h"
#include "GameSystem/ResultManager.h"

class AResultManager;

AGM::AGM()
{
	GameStateClass = AGS::StaticClass();
	PlayerStateClass = APS::StaticClass();
	PlayerControllerClass = APC::StaticClass();
	DefaultPawnClass = AMyCharacter::StaticClass();
}

/*
 * 기능 1 : 서버에 플레이어가 접속할 때 마다 호출
 * 기능 2 : 2명 접속 확인하면 역할 담당
 * 순서 : 플레이어가 서버 접속-> 게임모드가 PC 생성 -> 서버가 PC를 게임모드 등록-> 이때 PostLogin 호출됨
 */
void AGM::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (!PlayerStateClass ||!GameStateClass) return;	
	
	// 2인일 때 역할 할당됨
	if (GetNumPlayers() == 2)
	{
		if (AssignPlayerTypes())
		{
			if (AGS* Gs = GetGameState<AGS>())
			{
				float ServerNow = UGameplayStatics::GetTimeSeconds(GetWorld());
				float MathStartTime = ServerNow + 5.5f;
				Gs->MatchStartTime = MathStartTime;

				WaitForAllClientsReadyAndStart();
				StartMatchTimerWithDelay(5.5f);
				FTimerHandle PaintEnableHandle;
				GetWorld()->GetTimerManager().SetTimer(PaintEnableHandle, [this]()
				{
					MakeCharacterPaintReady(true);
				
				}, 5.5f, false); // ← 5.5초 후 단 한 번만 실행				
			}
		}
	}
}

void AGM::OnMatchTimeEnded()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APC* PC = Cast<APC>(It->Get()))
		{
			PC->Client_ShowResultUI();
			PC->RemoveCrossWidget();
			PC->RemoveTimerWidget();
			PC->Client_StopBGM();
			PC->RemoveMinimapWidget();
		}
	}
	for (TActorIterator<AMyCharacter> It(GetWorld()); It; ++It)
	{
		if (AMyCharacter* MyChar = *It)
		{
			MyChar->HidePlayerVisuals();
		}
	}
	for (TActorIterator<AResultManager> It(GetWorld()); It; ++It)
	{
		if (AResultManager* RM = *It)
		{
			UE_LOG(LogTemp, Warning, TEXT("ResultManager 찾음: %s"), *RM->GetName());
			RM->SnapshotPaintMap();
			MakeCharacterPaintReady(false);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT(" ResultManager가 nullptr임"));
		}
		break; // 하나만 있겠지?
	}
}

void AGM::MakeCharacterPaintReady(bool bReady)
{
	for (TActorIterator<AMyCharacter> It(GetWorld()); It; ++It)
	{
		AMyCharacter* Char = *It;
		if (Char)
		{
			Char->SetCanPaint(bReady);
		}
	}
}

void AGM::CallEndingSequence()
{
	// BP_EndingSequenceTrigger를 찾아서 이벤트 호출
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor->ActorHasTag(FName("Seq"))) 
		{
			Actor->CallFunctionByNameWithArguments(TEXT("PlayEndingSeq"), *GLog, nullptr, true);
			break;
		}
	}
}


/*
 * 기능1 : 클라이언트들이 bIsReadyToStart = true로 설정되었는지 확인
 * 기능2 : 두 명 모두 준비되었을 때 동시에 ShowStartWidgetAndStartAnim()을 실행해서 스타트위젯 Visible처리 && 애니메이션 틀기
 */
void AGM::WaitForAllClientsReadyAndStart()
{
	FTimerHandle RetryHandle;
	GetWorld()->GetTimerManager().SetTimer(RetryHandle, [this]()
	{
		// 준비된 PC 모으기 위한 배열
		TArray<APC*> ReadyPCs;
		int32 TotalPlayers = 0;

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APC* PC = Cast<APC>(It->Get()))
			{
				TotalPlayers++;
				if (PC->bIsReadyToStart)
				{
					ReadyPCs.Add(PC);
				}
			}
		}

		if (ReadyPCs.Num() == TotalPlayers && TotalPlayers == 2)
		{			
			FTimerHandle DelayHandle;
			GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this, ReadyPCs]()
			{
				for (APC* PC : ReadyPCs)
				{
					PC->Multicast_ShowStartWidgetAndStartAnim();
					PC->Multicast_ShowMinimap();
				}
			}, 0.2f, false);		
		}
		else
		{
			// 다시 대기
			WaitForAllClientsReadyAndStart();
		}
	}, 0.2f, false);
}

/**
 * 기능1 : 매치 타이머 액터를 Delay만큼 기다렸다가 시작함 -> 왜 딜레이? 스타트위젯에서 5.5초 애니메이션 있어서
 */
void AGM::StartMatchTimerWithDelay(float Delay)
{
	FTimerHandle StartHandle;
	GetWorld()->GetTimerManager().SetTimer(StartHandle, [this]()
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APC* PC = Cast<APC>(It->Get()))
			{
				PC->Client_StartRandomBGM(); // 
			}
		}

		for (TActorIterator<AMatchTimer> It(GetWorld()); It; ++It)
		{
			It->StartCountDown();
			break;
		}
	}, Delay, false);
}

/*
 * 기능1:  플레이어스테이트를 게임스테이트에 배열로 저장
 * 기능2:  플레이어스테이트2개면(== 접속2인) 역할 랜덤 지정 
 */
bool AGM::AssignPlayerTypes()
{
	bool bSuccess = false;
	
	if (AGS* Gs = GetGameState<AGS>())
	{
		TArray<APS*> PlayerStates;		
		for (APlayerState* PlayerState : Gs->PlayerArray)
		{
			if (APS* MyPs = Cast<APS>(PlayerState))
			{
				PlayerStates.Add(MyPs);
			}
		}
		
		if (PlayerStates.Num() == 2)
		{
			int32 RandomIndex = FMath::RandBool() ? 0:1;
			PlayerStates[RandomIndex] ->PlayerType = EPlayerType::Smoker;
			PlayerStates[1- RandomIndex] ->PlayerType = EPlayerType::NonSmoker;

			// 각 플레이어의 캐릭터를 찾아서 SetPlayerType 호출
			for (APS* Ps : PlayerStates)
			{
				if (Ps && Ps->GetPawn()) // PlayerState가 캐릭터를 소유하고 있는지 확인
				{
					if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(Ps->GetPawn()))
					{
						MyCharacter->SetPlayerType(Ps->PlayerType);
						MyCharacter->SpawnAndAttachWeapon(Ps->PlayerType);
						// UE_LOG(LogTemp, Warning, TEXT("AssignPlayerTypes → [%s] PlayerType: %d"), *MyCharacter->GetName(), (int32)Ps->PlayerType);

						if (Ps->PlayerType == EPlayerType::Smoker)
						{
							MyCharacter->bIsSmoker=true;
						}
						else if (Ps->PlayerType == EPlayerType::NonSmoker)
						{
							MyCharacter->bIsSmoker=false;	
						}
					}
				}
			}
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("현재 플레이어 수: %d, 2명이 아닙니다."), PlayerStates.Num());
			return false;
		}		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameState를 가져오지 못했습니다."));
		return false;
	}	
}

/*
 * 기능1 : 레벨에 있는 미니언들 배열에 다 때려넣음
 * 기능2: 그 배열에 있는 미니언 중 지정 반경 안에 있는 미니언들한테 GoToLocation 호출때림
 */
void AGM::AlertMinion(class AActor* AlertInstigator, const FVector& Location, const float Radius)
{
	TArray<AActor*> Minions;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMinion::StaticClass(), Minions);

	for (const auto Minion : Minions)
	{
		if (AlertInstigator == Minion)
		{
			continue;
		}
		if (const auto Distance = FVector::Distance(AlertInstigator->GetActorLocation(), Minion->GetActorLocation()); Distance < Radius)
		{
			if (const auto MinionCharacter = Cast<AMinion>(Minion))
			{
				MinionCharacter->GotoLocation(Location);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
		TEXT("Alert Done!"));	
			}
		}
	}
}




