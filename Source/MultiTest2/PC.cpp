// Fill out your copyright notice in the Description page of Project Settings.


#include "PC.h"
#include "EngineUtils.h"
#include "GM.h"
#include "GS.h"
#include "MyCharacter.h"
#include "NavigationSystemTypes.h"
#include "Components/AudioComponent.h"
#include "Components/TextBlock.h"
#include "GameSystem/ResultManager.h"
#include "Kismet/GameplayStatics.h"
#include "UI/CrossHair.h"
#include "UI/MainHUD.h"
#include "UI/Minimap.h"
#include "UI/StartAlert.h"
#include "UI/Timer.h"
#include "UI/Result.h"
#include "UI/Minimap.h"


class AGM;
class AGS;

void APC::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController())
	{
		if (TimerWidgetClass) CreateTimerWidget();
		if (StartAlertWidgetClass) CreateStartWidget();
		if (CrossWidgetClass) CreateCrossWidget();
		if (ResultWidgetClass) CreateResultWidget();
		if (MinimapWidgetClass) CreateMinimapWidget();
		// 모든 위젯 생성 완료 후 준비 완료 플래그 전송
		Server_NotifyClientReady();		
	}
}

void APC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateTimerWidget();
}

void APC::CreateTimerWidget()
{	
	TimerWidget = CreateWidget<UTimer>(this, TimerWidgetClass);
	if (TimerWidget)
	{
		TimerWidget->AddToViewport(/*ZOrder=*/0); // 먼저 아래쪽에 추가
	}
}

void APC::CreateCrossWidget()
{
	if (!CrossWidget && CrossWidgetClass)
	{
		CrossWidget = CreateWidget<UCrossHair>(this, CrossWidgetClass);
		if (CrossWidget)
		{
			CrossWidget->AddToViewport(0);
			CrossWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void APC::CreateResultWidget()
{
	ResultWidget = CreateWidget<UResult>(this, ResultWidgetClass);
	if (ResultWidget)
	{
		ResultWidget->AddToViewport();
		ResultWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APC::CreateMinimapWidget()
{
	MinimapWidget = CreateWidget<UMinimap>(this, MinimapWidgetClass);
	if (MinimapWidget)
	{
		MinimapWidget->AddToViewport();
		MinimapWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APC::RemoveCrossWidget_Implementation()
{
	if (CrossWidget)
	{
		CrossWidget->RemoveFromParent();
		CrossWidget = nullptr;
	}
}

void APC::RemoveTimerWidget_Implementation()
{
	if (TimerWidget)
	{
		TimerWidget->RemoveFromParent();
		TimerWidget = nullptr;
	}
}

void APC::CreateStartWidget()
{
	if (!StartAlertWidget && StartAlertWidgetClass)
	{
		StartAlertWidget = CreateWidget<UStartAlert>(this, StartAlertWidgetClass);
		if (StartAlertWidget)
		{
			StartAlertWidget->AddToViewport(1);
			StartAlertWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}	
}

void APC::RemoveMinimapWidget_Implementation()
{
	if (MinimapWidget)
	{
		MinimapWidget->RemoveFromParent();
		MinimapWidget = nullptr;
	}
}

void APC::ShowStartWidgetAndStartAnim()
{
	if (StartAlertWidget && !bStartAlertShown )
	{
		bStartAlertShown = true;
		StartAlertWidget->SetVisibility(ESlateVisibility::Visible);
		StartAlertWidget->StartAnim();
		UE_LOG(LogTemp, Log, TEXT("서버에서 스타트 위젯 표시 + 애니 실행"));
	}
}
void APC::Client_ShowStartWidgetAndStartAnim_Implementation()
{
	if (GetNetMode() == NM_ListenServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("서버 클라 자기 자신이므로 무시"));
		return;
	}

	if (StartAlertWidget && !StartAlertWidget->IsInViewport())
	{
		StartAlertWidget->AddToViewport();
		StartAlertWidget->StartAnim();
	}
}

/*
 * Why Multicast?
 * Multicast는 서버에서 한 번만 호출하면 모든 클라이언트에서 동시에 실행
 * 클라에서 Client RPC 쓰면 서버가 각 클라에게 따로 따로 호출하게 되어 시간차 생김.
 * IsLocalController()로 분기하면 서버에서만 먼저 보여버림.
 */
void APC::Multicast_ShowStartWidgetAndStartAnim_Implementation()
{
	// 약간의 딜레이 후 실행
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, [this]()
	{
		if (StartAlertWidget && !bStartAlertShown)
		{
			bStartAlertShown = true;
			StartAlertWidget->SetVisibility(ESlateVisibility::Visible);
			StartAlertWidget->StartAnim();
		}
		if (CrossWidget)
		{
			FTimerHandle CrossHairDelayHandle;
			GetWorld()->GetTimerManager().SetTimer(CrossHairDelayHandle, [this]()
			{
				CrossWidget->SetVisibility(ESlateVisibility::Visible);
			}, 5.5f, false);
		}
		
	}, 0.05f, false);
	

	
}

void APC::ToggleMinimap()
{
	bMinimapVisible = !bMinimapVisible;
	if (MinimapWidget)
	{
		MinimapWidget->SetVisibility(bMinimapVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void APC::Multicast_ShowMinimap_Implementation()
{
	bMinimapVisible = true;
	if (MinimapWidget)
	{
		MinimapWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void APC::UpdateTimerWidget()
{
	if (IsLocalController() && TimerWidget)
	{
		if (AGS* Gs = GetWorld()->GetGameState<AGS>())
		{
			TimerWidget->UpdateTimeText(Gs->GetFormattedTime()); // 내부에서 00:55 포맷
		}
	}
}

void APC::PlayRandomBGM()
{
	if (!BGMComponent || BGMSoundList.Num() == 0) return;

	int32 RandIndex = FMath::RandRange(0, BGMSoundList.Num() - 1);
	USoundBase* SelectedBGM = BGMSoundList[RandIndex];

	if (SelectedBGM)
	{
		BGMComponent->SetSound(SelectedBGM);
		BGMComponent->Play();
	}
}

void APC::UpdatePaintUI()
{
	if (!MinimapWidget) return;

	if (AGS* Gs = GetWorld()->GetGameState<AGS>())
	{
		APawn* MyPawn = GetPawn();
		if (!MyPawn) return;

		if (AMyCharacter* MyChar = Cast<AMyCharacter>(MyPawn))
		{
			const float Percent = (MyChar->PlayerType == EPlayerType::Smoker)
				? Gs->SmokerPaintPercent
				: Gs->NonSmokerPaintPercent;

			MinimapWidget->UpdatePaintPercent(Percent);
		}
	}
}

void APC::Client_StopBGM_Implementation()
{
	if (BGMComponent)
	{
		BGMComponent->OnAudioFinished.RemoveAll(this);

		// 부드럽게 끄기
		if (BGMComponent->IsPlaying())
		{
			BGMComponent->FadeOut(5.0f, 0.0f); // 5초 동안 서서히 꺼짐
		}

		// Destroy는 약간 뒤에 해줘야 함 (5초 후)
		FTimerHandle TempHandle;
		GetWorld()->GetTimerManager().SetTimer(TempHandle, [this]()
		{
			if (BGMComponent)
			{
				BGMComponent->DestroyComponent();
				BGMComponent = nullptr;
			}
		}, 5.1f, false); // FadeOut 시간보다 약간 여유 줌
	}
}

void APC::Client_StartRandomBGM_Implementation()
{
	if (BGMSoundList.Num() == 0) return;

	// 이미 생성된 컴포넌트가 있다면 제거
	if (BGMComponent)
	{
		BGMComponent->Stop();
		BGMComponent->DestroyComponent();
	}

	// 새 AudioComponent 생성
	BGMComponent = NewObject<UAudioComponent>(this);
	BGMComponent->bAutoActivate = false;
	BGMComponent->RegisterComponentWithWorld(GetWorld());

	// 끝났을 때 다음 곡 재생 델리게이트 연결
	BGMComponent->OnAudioFinished.AddDynamic(this, &APC::PlayRandomBGM);

	// 첫 곡 재생
	PlayRandomBGM();
}

void APC::Client_NotifyResultWidgetClosed_Implementation()
{
	if (AGM* GM = GetWorld()->GetAuthGameMode<AGM>())
	{
		GM->CallEndingSequence(); //  여기서 시퀀서 재생
	}
}

void APC::Server_NotifyResultWidgetClosed_Implementation()
{
	Client_NotifyResultWidgetClosed();
}

void APC::Client_UpdateResultUI_Implementation(float InSmokerRate, float InNonSmokerRate)
{
	if (!ResultWidget) return;

	ResultWidget->SetVisibility(ESlateVisibility::Visible);
	ResultWidget->TargetSmokerRate = InSmokerRate;
	ResultWidget->TargetNonSmokerRate = InNonSmokerRate;
}

void APC::Client_ShowResultUI_Implementation()
{
	if (!IsLocalController()) return;
	
	if (AMyCharacter* MyChar = Cast<AMyCharacter>(GetPawn()))
	{
		MyChar->RemoveUltWidget();
		//UE_LOG(LogTemp, Log, TEXT("RemoveUltWG"));
	}
	if (ResultWidget)
	{
		ResultWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void APC::Server_NotifyClientReady_Implementation()
{
	bIsReadyToStart = true;	
}








