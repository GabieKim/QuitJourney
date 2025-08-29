// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerIcon.generated.h"

/**
 * 
 */
class AMyCharacter;
UCLASS()
class MULTITEST2_API UPlayerIcon : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_WaterDrop;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Cigarrette;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Question;
	
	
public:
	/** 버튼 클릭 이벤트를 바인딩하는 함수 */
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
	void SetRoleIcon(EPlayerType NewType);
	
};
