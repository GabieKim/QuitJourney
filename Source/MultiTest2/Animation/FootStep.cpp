// Fill out your copyright notice in the Description page of Project Settings.


#include "../Animation/FootStep.h"
#include "Kismet/GameplayStatics.h"
#include "../MyCharacter.h"
#include "Components/SceneCaptureComponent2D.h"

void UFootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
	if (AMyCharacter* Char = Cast<AMyCharacter>(MeshComp->GetOwner()))
	{
		Char->PlayFootstep(FootstepType); // ← 여기서 캐릭터 쪽 재생 함수 호출	
	}
}
