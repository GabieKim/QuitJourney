// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Result.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST2_API UResult : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, meta =(BindWidget), Category="TXT")
	class UTextBlock* Txt_SmokerValue;
	UPROPERTY(VisibleAnywhere, meta =(BindWidget), Category="TXT")
	class UTextBlock* Txt_NonSmokerValue;

	UPROPERTY(VisibleAnywhere, meta =(BindWidget), Category="PB")
	class UProgressBar* PB_Smoker;
	UPROPERTY(VisibleAnywhere, meta =(BindWidget), Category="PB")
	class UProgressBar* PB_NonSmoker;
	UPROPERTY(VisibleAnywhere, meta =(BindWidget), Category="IMG")
	class UImage* Img_CapturedMap;	
	UPROPERTY(meta = (BindWidget))
	class UEndingText* EndingTextWidget;
	UPROPERTY()
	class AResultManager* ResultManager;
	
	UPROPERTY()
	float TargetSmokerRate = 0.f;
	UPROPERTY()
	float TargetNonSmokerRate = 0.f;	
	UPROPERTY()
	bool bResultHandled = false;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	USoundBase* SmokerWinSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	USoundBase* SmokerLoseSound;
	
	UFUNCTION()
	void PlayEndingTextSound(bool bSmokerWin);
};
