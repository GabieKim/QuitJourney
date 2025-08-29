// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MatchTimer.generated.h"


UCLASS()
class MULTITEST2_API AMatchTimer : public AActor
{
	GENERATED_BODY()
	
public:	
	AMatchTimer();

	UFUNCTION(BlueprintCallable, Category = "Timer")
	FString GetFormattedTime() const;
	float GetRemainingTime() const;

	
protected:
	virtual void BeginPlay() override;
	//virtual void Tick(float DeltaTime) override;
	
	
public:
	UFUNCTION()
	void StartCountDown();
	UFUNCTION()
	void EndMatch();

	UFUNCTION()
	void CountdownTick();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY()
	class UTimer* TimerWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTimer> TimerWidgetClass;
	
private:
	UPROPERTY(EditAnywhere, Category = "TImer")
	float MatchDuration = 120.f;
	UPROPERTY()
	float RemainingTime;
	UPROPERTY(EditAnywhere, Category = "Match")
	float StartDelay = 5.f;
	UPROPERTY(EditAnywhere, Category = "Match")
	bool bHasGameStarted = false;
	UPROPERTY(EditAnywhere, Category = "Match")
	bool bMatchStartScheduled = false;
	
	FTimerHandle MatchTimerHandle;
	FTimerHandle StartDelayHandle;
	FTimerHandle CountdownTickHandle;
	FTimerHandle PlayerCheckHandle;

	
	
	
};


