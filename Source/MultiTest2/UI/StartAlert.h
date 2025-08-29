// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartAlert.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST2_API UStartAlert : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* CountdownAnim;

public:
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable)
	void SetOwningPC(class APC* InPC);
	
	UPROPERTY()
	APC* OwningPC;
	
	UFUNCTION(BlueprintCallable)
	void StartAnim();
};
