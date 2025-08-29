// Fill out your copyright notice in the Description page of Project Settings.


#include "../UI/Ult.h"
#include "Components/RadialSlider.h"

void UUlt::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (RS_Ultimate)
	{
		// 맨 처음은 0으로 셋팅함
		RS_Ultimate->SetValue(0.0f);
		//UE_LOG(LogTemp, Warning, TEXT("Ult::NativeConstruct"));
	}
}

/*
 * 기능 : 상대방 때린 횟수 5회면 궁극기 풀로 채워 업데이트
 * 매개변수 : 상대방 때린 횟수
 */
void UUlt::UpdateUltimateCharge(float CurrentHits)
{
	if (RS_Ultimate)
	{
		float ChargePercent = FMath::Clamp(CurrentHits / 5.0f, 0.0f, 1.0f);
		//UE_LOG(LogTemp, Warning, TEXT("궁극기 충전률: %f"), ChargePercent); 
		RS_Ultimate->SetValue(ChargePercent);
		RS_Ultimate->SynchronizeProperties(); 
	}
}
