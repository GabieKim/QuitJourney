// Fill out your copyright notice in the Description page of Project Settings.


#include "../UI/Minimap.h"
#include "Components/TextBlock.h"

void UMinimap::UpdatePaintPercent(float Percent)
{
	if (!Txt_PaintPercent) return;

	// 퍼센트를 0~100으로 변환하고 소수점 1자리로 표시
	const int32 ClampedPercent = FMath::Clamp(Percent * 100.f, 0.f, 100.f);
	const FString DisplayText = FString::Printf(TEXT("%d%%"), ClampedPercent);
	
	Txt_PaintPercent->SetText(FText::FromString(DisplayText));
}
