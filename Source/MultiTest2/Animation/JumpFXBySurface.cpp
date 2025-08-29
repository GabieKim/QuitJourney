// Fill out your copyright notice in the Description page of Project Settings.


#include "../Animation/JumpFXBySurface.h"
#include "../MyCharacter.h" 
#include "NiagaraComponent.h"

void UJumpFXBySurface::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
	// 1. 내 캐릭터만 조작 (클라이언트 기준)
	AMyCharacter* Char = Cast<AMyCharacter>(MeshComp->GetOwner());
	if (!Char || !Char->IsLocallyControlled()) return; // 중요: 내 클라만 색 읽고 사운드 결정

	// 2. RenderTarget은 이 캐릭터 본인의 것을 사용해야 함
	ESurfaceType SurfaceType = Char->GetSurfaceTypeUnderFoot();

	// 3. 색 결정 (이전처럼)
	FLinearColor EffectColor = FLinearColor::White;
	switch (SurfaceType)
	{
	case ESurfaceType::SmokerPainted:
		EffectColor = SmokerColor;
		break;
	case ESurfaceType::NonsmokerPainted:
		EffectColor = NonsmokerColor;
		break;
	default:
		EffectColor = DefaultColor;
		break;
	}

	// 4. 본인의 Niagara 이펙트 조작
	if (Char->DustFX)
	{
		Char->DustFX->SetVariableLinearColor(FName("User.JumpColor"), EffectColor);
		Char->DustFX->Activate(true);
		//UE_LOG(LogTemp, Warning, TEXT("DustFX Activate"));
	}
	
}
