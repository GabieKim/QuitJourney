// Fill out your copyright notice in the Description page of Project Settings.


#include "../GameSystem/MatchTimer.h"
#include "EngineUtils.h"
#include "MultiTest2/GS.h"
#include "Net/UnrealNetwork.h"
#include "../GM.h"

AMatchTimer::AMatchTimer()
{
	PrimaryActorTick.bCanEverTick = false; // 틱 안 써서 false;
	bReplicates = true;
}

FString AMatchTimer::GetFormattedTime() const
{
	int32 Minutes = FMath::FloorToInt(RemainingTime / 60.f);
	int32 Seconds = FMath::FloorToInt(RemainingTime) % 60;

	return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}

float AMatchTimer::GetRemainingTime() const
{
	return RemainingTime;
}

void AMatchTimer::BeginPlay()
{
	Super::BeginPlay();
}

void AMatchTimer::StartCountDown()
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("StartCountDown() called but no authority"));	
		return; // 서버만 실행
	}
	//UE_LOG(LogTemp, Display, TEXT("StartCountDown() called"));
	RemainingTime = MatchDuration;

	//UE_LOG(LogTemp, Display, TEXT("타이머 시작! 남은 시간: %.0f초"), RemainingTime);

	GetWorld()->GetTimerManager().SetTimer(
		CountdownTickHandle,
		this,
		&AMatchTimer::CountdownTick,
		1.0f,
		true
	);

	GetWorld()->GetTimerManager().SetTimer(
		MatchTimerHandle,
		this,
		&AMatchTimer::EndMatch,
		MatchDuration,
		false
	);
}

void AMatchTimer::EndMatch()
{
	UE_LOG(LogTemp, Display, TEXT("게임 종료!"));

	// 게임 상태 변경 or UI 출력 등 처리
	// 예: 게임 모드에 알려주기
	if (AGM* GM = Cast<AGM>(GetWorld()->GetAuthGameMode()))
	{
		GM->OnMatchTimeEnded();
	}
}


void AMatchTimer::CountdownTick()
{
	if (RemainingTime <= 0.f)
	{
		// 타이머 중지
		GetWorld()->GetTimerManager().ClearTimer(CountdownTickHandle);
		EndMatch();
		return;
	}

	RemainingTime -= 1.f;
	RemainingTime = FMath::Max(RemainingTime, 0.f);
	
	if (AGS* Gs = GetWorld()->GetGameState<AGS>())
	{
		Gs->CurrentRemainingTime = RemainingTime;
	}
	const FString Formatted = GetFormattedTime();
	

	UE_LOG(LogTemp, Display, TEXT("남은 시간: %s"), *Formatted);

}



void AMatchTimer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

