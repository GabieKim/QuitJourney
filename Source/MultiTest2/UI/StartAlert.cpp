// Fill out your copyright notice in the Description page of Project Settings.


#include "../UI/StartAlert.h"
#include "../GameSystem/MatchTimer.h"

void UStartAlert::NativeConstruct()
{
	Super::NativeConstruct();	
}

void UStartAlert::SetOwningPC(class APC* InPC)
{
	OwningPC = InPC;
}

void UStartAlert::StartAnim()
{
	if (CountdownAnim)
	{
		PlayAnimation(CountdownAnim);
	}
}

