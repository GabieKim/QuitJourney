// Fill out your copyright notice in the Description page of Project Settings.


#include "../Seq/SeqActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "MultiTest2/GI.h"

ASeqActor::ASeqActor()
{
	PrimaryActorTick.bCanEverTick = true;

	MySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SetRootComponent(MySkeletalMesh);
	
	PlayerType = EPlayerType::None;
}

void ASeqActor::BeginPlay()
{
	Super::BeginPlay();
			
}

void ASeqActor::SetSkinIdx(int32 Value)
{
	if (Value < 0) return;  
	if (!CharacterSkinDataTable) return;  

	// 현재 선택된 스킨을 가져옴
	CharacterSkins = GetSkinByIndex(Value);
	if (!CharacterSkins) return; 

	// 머티리얼 리스트 가져오기
	TArray<UMaterialInterface*> Materials = CharacterSkins->GetMaterialArray();

	// 머티리얼이 있을 때만 적용
	for (int32 i = 0; i < Materials.Num(); i++)
	{
		if (Materials[i])
		{
			GetSkeletalMeshComponent()->SetMaterial(i, Materials[i]);
		}
	}
}

FCharacterSkins* ASeqActor::GetSkinByIndex(int32 Index) const
{
	if (!CharacterSkinDataTable) return nullptr;  // 데이터 테이블이 없으면 nullptr 반환

	TArray<FCharacterSkins*> CharacterSkinsRows;
	CharacterSkinDataTable->GetAllRows<FCharacterSkins>(TEXT("MyCharacter"), CharacterSkinsRows);

	if (CharacterSkinsRows.IsValidIndex(Index))  //  유효한 인덱스인지 확인
	{
		return CharacterSkinsRows[Index];
	}

	return nullptr;  //  잘못된 인덱스면 nullptr 반환
}

void ASeqActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (MySkinIdx == -1)
	{
		const auto MyGi = Cast<UGI>(GetWorld()->GetGameInstance());

		if (MyGi && MyGi->SkinIdxMap.Contains(PlayerType))
		{
			MySkinIdx = MyGi->SkinIdxMap[PlayerType];
			SetSkinIdx(MySkinIdx);

			UE_LOG(LogTemp, Warning, TEXT("[SeqActor] SkinIdx updated for %d: %d"), (int32)PlayerType, MySkinIdx);
			PrimaryActorTick.bCanEverTick = false; 
		}
	}
}

