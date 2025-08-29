// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Minimap.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST2_API UMinimap : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Background;

	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Minimap;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_PaintPercent;
	
public:
	UFUNCTION(BlueprintCallable)
	void UpdatePaintPercent(float Percent);
};
