// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GM.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST2_API AGM : public AGameMode
{
	GENERATED_BODY()

	AGM();
	
protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	
	
public:
	UFUNCTION()
	void WaitForAllClientsReadyAndStart();
	UFUNCTION()
	void StartMatchTimerWithDelay(float Delay);
	UFUNCTION(BlueprintCallable, Category="Minion")
	void AlertMinion(class AActor* AlertInstigator, const FVector& Location, const float Radius);
	UFUNCTION()
	bool AssignPlayerTypes();
	UFUNCTION()
	void OnMatchTimeEnded();
	UFUNCTION()
	void MakeCharacterPaintReady(bool bReady);
	UFUNCTION()
	void CallEndingSequence();
	
};
