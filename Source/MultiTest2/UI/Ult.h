// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ult.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST2_API UUlt : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	class URadialSlider* RS_Ultimate;
	
public:
	/** 버튼 클릭 이벤트를 바인딩하는 함수 */
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
	void UpdateUltimateCharge(float CurrentHits);
};
