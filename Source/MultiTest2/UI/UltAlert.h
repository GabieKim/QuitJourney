// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UltAlert.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST2_API UUltAlert : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_UltAlert;
	
public:
	/** 버튼 클릭 이벤트를 바인딩하는 함수 */
	virtual void NativeConstruct() override;
};
