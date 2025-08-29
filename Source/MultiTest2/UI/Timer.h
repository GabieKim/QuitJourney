// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Timer.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST2_API UTimer : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_RemainingTime;
	
public:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void UpdateTimeText(const FString& TimeText);
};
