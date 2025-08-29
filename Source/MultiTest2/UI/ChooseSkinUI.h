// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChooseSkinUI.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST2_API UChooseSkinUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* PrevBtn;
	UPROPERTY(meta = (BindWidget))
	class UButton* NextBtn;
	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmBtn;

public:
	/** 버튼 클릭 이벤트를 바인딩하는 함수 */
	virtual void NativeConstruct() override;	

protected:
	/** 버튼 클릭 이벤트 함수 */
	UFUNCTION()
	void OnPrevClicked();
	UFUNCTION()
	void OnNextClicked();
	UFUNCTION()
	void OnConfirmClicked();

	UPROPERTY()
	TObjectPtr<class AMainMenuPawn> MainMenuPawn;
	
};
