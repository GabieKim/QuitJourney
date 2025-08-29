// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCalcPaintDele);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCaptureDele);

UCLASS()
class MULTITEST2_API AMainHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void PreInitializeComponents() override;
	//virtual void BeginPlay() override;

public:

	/*델리게이트*/
	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "Result")
	FCalcPaintDele OnCalcPaintRequest;
	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "Result")
	FCaptureDele OnCapturedRequest;
	

};
