// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GS.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST2_API AGS : public AGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(ReplicatedUsing = OnRep_MatchStartTime)
	float MatchStartTime = -1.f;

	UFUNCTION()
	void OnRep_MatchStartTime();

	UPROPERTY(Replicated)
	float CurrentRemainingTime = 60.f;

	// 남은 시간을 문자열로 반환 (예: "00:55")
	FString GetFormattedTime() const;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bSmokerWin = false;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated)
	float SmokerPaintPercent = 0.f;
	UPROPERTY(Replicated)
	float NonSmokerPaintPercent = 0.f;

	UFUNCTION()
	void SetPaintRatio(int32 SmokerPixel, int32 NonSmokerPixel, int32 TotalPixel);
};
