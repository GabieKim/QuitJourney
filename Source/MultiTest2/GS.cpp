// Fill out your copyright notice in the Description page of Project Settings.


#include "GS.h"

#include "PC.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AGS::OnRep_MatchStartTime()
{
	float LocalNow = UGameplayStatics::GetTimeSeconds(GetWorld());
	float Delay = FMath::Max(0.f, MatchStartTime - LocalNow);
}

FString AGS::GetFormattedTime() const
{
	int32 Minutes = FMath::FloorToInt(CurrentRemainingTime / 60.f);
	int32 Seconds = FMath::FloorToInt(CurrentRemainingTime) % 60;

	return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}

void AGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGS, MatchStartTime);
	DOREPLIFETIME(AGS, CurrentRemainingTime);
	DOREPLIFETIME(AGS, bSmokerWin);
	DOREPLIFETIME(AGS, SmokerPaintPercent);
	DOREPLIFETIME(AGS, NonSmokerPaintPercent);
}

void AGS::SetPaintRatio(int32 SmokerPixel, int32 NonSmokerPixel, int32 TotalPixel)
{
	if (TotalPixel <= 0)
	{
		SmokerPaintPercent = 0.f;
		NonSmokerPaintPercent = 0.f;
		return;
	}

	SmokerPaintPercent = static_cast<float>(SmokerPixel) / TotalPixel;
	NonSmokerPaintPercent = static_cast<float>(NonSmokerPixel) / TotalPixel;

	UE_LOG(LogTemp, Warning, TEXT("[GS] Smoker: %.1f%% | NonSmoker: %.1f%%"),
		SmokerPaintPercent * 100.f, NonSmokerPaintPercent * 100.f);
}
