// Fill out your copyright notice in the Description page of Project Settings.


#include "../UI/PlayerIcon.h"
#include "Components/Image.h"
#include "MultiTest2/MyCharacter.h"

void UPlayerIcon::NativeConstruct()
{
	Super::NativeConstruct();
	
	Img_WaterDrop->SetVisibility(ESlateVisibility::Hidden);
	Img_Cigarrette->SetVisibility(ESlateVisibility::Hidden);
	Img_Question->SetVisibility(ESlateVisibility::Visible);
	//UE_LOG(LogTemp, Warning, TEXT("PlayerIcon 위젯 생성됨!"));
}

void UPlayerIcon::SetRoleIcon(EPlayerType NewType)
{
	//UE_LOG(LogTemp, Warning, TEXT("SetRoleIcon: %s"), bIsSmoker ? TEXT("Smoker") : TEXT("NonSmoker"));
	// 모든 아이콘 숨김으로 초기화
	Img_Cigarrette->SetVisibility(ESlateVisibility::Hidden);
	Img_WaterDrop->SetVisibility(ESlateVisibility::Hidden);
	Img_Question->SetVisibility(ESlateVisibility::Hidden);

	switch (NewType)
	{
	case EPlayerType::Smoker:
		Img_Cigarrette->SetVisibility(ESlateVisibility::Visible);
		break;
	case EPlayerType::NonSmoker:
		Img_WaterDrop->SetVisibility(ESlateVisibility::Visible);
		break;
	default:
		Img_Question->SetVisibility(ESlateVisibility::Visible);
		break;
	}
}
