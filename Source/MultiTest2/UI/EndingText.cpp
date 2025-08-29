// Fill out your copyright notice in the Description page of Project Settings.


#include "../UI/EndingText.h"
#include "Components/TextBlock.h"

void UEndingText::NativeConstruct()
{
	Super::NativeConstruct();

	if (TB_QuitSucText)
		TB_QuitSucText->SetVisibility(ESlateVisibility::Hidden);

	if (TB_QuitFailText)
		TB_QuitFailText->SetVisibility(ESlateVisibility::Hidden);
}
void UEndingText::ShowQuitSucText()
{
	TB_QuitSucText->SetVisibility(ESlateVisibility::Visible);
	TB_QuitFailText->SetVisibility(ESlateVisibility::Hidden);	
}

void UEndingText::ShowQuitFailText()
{
	TB_QuitFailText->SetVisibility(ESlateVisibility::Visible);
	TB_QuitSucText->SetVisibility(ESlateVisibility::Hidden);
}
