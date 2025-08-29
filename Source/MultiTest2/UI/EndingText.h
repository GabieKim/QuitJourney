// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndingText.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST2_API UEndingText : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_QuitFailText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_QuitSucText;
	
public:
	UFUNCTION(BlueprintCallable)
	void ShowQuitSucText();
	UFUNCTION(BlueprintCallable)
	void ShowQuitFailText();
};
