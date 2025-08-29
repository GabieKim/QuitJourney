// Fill out your copyright notice in the Description page of Project Settings.


#include "../UI/ChooseSkinUI.h"
#include "Components/Button.h"
#include "../MainMenuPawn.h"
#include "Kismet/GameplayStatics.h"

void UChooseSkinUI::NativeConstruct()
{
	Super::NativeConstruct();

	MainMenuPawn = Cast<AMainMenuPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	// 버튼이 유효하면 이벤트 바인딩
	if (PrevBtn)
	{
		PrevBtn->OnClicked.AddDynamic(this, &UChooseSkinUI::OnPrevClicked);
	}
	if (NextBtn)
	{
		NextBtn->OnClicked.AddDynamic(this, &UChooseSkinUI::OnNextClicked);
	}
	if (ConfirmBtn)
	{
		ConfirmBtn->OnClicked.AddDynamic(this, &UChooseSkinUI::OnConfirmClicked);
	}
}

void UChooseSkinUI::OnPrevClicked()
{
	if (MainMenuPawn)
	{
		MainMenuPawn->ChangeSkinIndex(-1);  
	}
}

void UChooseSkinUI::OnNextClicked()
{
	if (MainMenuPawn)
	{
		MainMenuPawn->ChangeSkinIndex(1);  
	}
}

void UChooseSkinUI::OnConfirmClicked()
{
	if (MainMenuPawn)
	{
		// 현재 선택된 스킨 인덱스를 가져와서 적용 요청
		MainMenuPawn->ApplyCharacterSkin(MainMenuPawn->GetCurrentSkinIdx());
	}
}