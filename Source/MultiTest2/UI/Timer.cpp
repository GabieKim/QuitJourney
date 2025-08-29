// Fill out your copyright notice in the Description page of Project Settings.


#include "../UI/Timer.h"
#include "../GameSystem/MatchTimer.h"
#include "Components/TextBlock.h"

void UTimer::NativeConstruct()
{
	Super::NativeConstruct();
	if (TB_RemainingTime)
	{
		// 초기 텍스트 설정
		TB_RemainingTime->SetText(FText::FromString(TEXT("01:00"))); 
	}
}

void UTimer::UpdateTimeText(const FString& TimeText)
{
	if (TB_RemainingTime)
	{
		TB_RemainingTime->SetText(FText::FromString(TimeText));
	}
}
