// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FootStep.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EFootstepType : uint8
{
	Walk,
	Run,
	Land
};
UCLASS()
class MULTITEST2_API UFootStep : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Footstep")
	EFootstepType FootstepType = EFootstepType::Walk;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
