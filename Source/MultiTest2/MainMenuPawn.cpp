// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "CharacterSkins.h"
#include "GI.h"

AMainMenuPawn::AMainMenuPawn()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	RootComponent = Arrow;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
	Camera->SetRelativeLocation(FVector(450.0f, 90.0f, 160.0f));
	Camera->SetRelativeRotation(FRotator(-10.0f, 180.0f, .0f));

	SMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SMesh->SetupAttachment(RootComponent);
	SMesh->SetRelativeLocation(FVector(.0f, -30.0f, 90.f));
	// 메시는 BP에서 입히기

	CurrentSkinIdx = 0;
}

void AMainMenuPawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority()) // 서버에서만 실행
	{
		Server_RandomizeCharacterSkin();
	}
}

void AMainMenuPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMainMenuPawn::SetCharacterMaterials()
{
	if (!CharacterSkin) return;
	
	// 배열로 머티리얼 가져오기
	TArray<UMaterialInterface*> Materials = CharacterSkin->GetMaterialArray();
	
	// 반복문으로 `SetMaterial()` 자동 처리
	for (int32 i = 0; i < Materials.Num(); i++)
	{
		if (Materials[i])  // NULL 체크
		{
			SMesh->SetMaterial(i, Materials[i]);
		}
	}
}

FCharacterSkins* AMainMenuPawn::GetSkinByIndex(int32 Index) const
{
	if (!CharacterSkinsDataTable) return nullptr;

	// 데이터 테이블에서 모든 캐릭터 스킨을 가져옴
	TArray<FCharacterSkins*> CharacterSkinsRows;
	CharacterSkinsDataTable->GetAllRows<FCharacterSkins>(TEXT("AMainMenuPawn::GetSkinByIndex"), CharacterSkinsRows);

	// 유효한 인덱스인지 확인 후 반환
	return CharacterSkinsRows.IsValidIndex(Index) ? CharacterSkinsRows[Index] : nullptr;
}

int32 AMainMenuPawn::GetTotalSkinCount() const
{
	if (!CharacterSkinsDataTable) return 0;

	// 데이터 테이블에서 모든 캐릭터 스킨 정보를 가져옴
	TArray<FCharacterSkins*> CharacterSkinsRows;
	CharacterSkinsDataTable->GetAllRows<FCharacterSkins>(TEXT("AMainMenuPawn::GetTotalSkinCount"), CharacterSkinsRows);
	
	// 총 개수를 반환
	return CharacterSkinsRows.Num();
}

/*
 * 기능 : DT_캐릭터스킨에서 랜덤한 인덱스를 선택하여 해당 캐릭터 스킨을 가져오고, 스켈레탈 메시의 머티리얼을 설정
 *		모든 클라가 다른 컬러 갖게 하기 위해 서버 / 클라 나누었음
 *		서버가 랜덤 값 할당-> 클라에게 전파
 */
void AMainMenuPawn::Server_RandomizeCharacterSkin_Implementation()
{	
	if (!CharacterSkinsDataTable) return;

	int32 TotalSkins = GetTotalSkinCount();  
	if (TotalSkins > 0)
	{
		int32 NewIdx = FMath::RandRange(0, TotalSkins - 1);
		Multicast_RandomizeCharacterSkin(NewIdx);
	}
}

void AMainMenuPawn::Multicast_RandomizeCharacterSkin_Implementation(int32 NewIdx)
{
	if (!CharacterSkinsDataTable) return;

	// 불필요한 데이터 테이블 조회 제거 → GetSkinByIndex() 사용
	CurrentSkinIdx = NewIdx;
	CharacterSkin = GetSkinByIndex(NewIdx);
	SetCharacterMaterials();  // 머티리얼 변경을 하나의 함수에서 관리

	// 선택한 스킨을 GameInstance에 저장
	if (UGI* GameInstance = Cast<UGI>(GetGameInstance()))
	{
		GameInstance->SkinIdx = NewIdx;
	}
}

void AMainMenuPawn::ChangeSkinIndex(int32 Direction)
{
	int32 SkinCount = GetTotalSkinCount();
	if (SkinCount <= 0) return;

	// 디렉션이 -1이면 Prev 방향 <-> +1이면 Next 방향 (순환인덱스)
	CurrentSkinIdx = (CurrentSkinIdx + Direction + SkinCount) % SkinCount;
	UpdateSkinPreview(CurrentSkinIdx);
}

void AMainMenuPawn::ApplyCharacterSkin(int32 NewIdx)
{
	// 현재 선택한 스킨 인덱스 업데이트
	CurrentSkinIdx = NewIdx;

	// 서버라면 바로 실행, 클라이언트라면 서버에 요청
	if (HasAuthority()) 
	{
		Multicast_ApplyCharacterSkin(NewIdx);
	}
	else  
	{
		Server_ApplyCharacterSkin(NewIdx);
	}
}

void AMainMenuPawn::Server_ApplyCharacterSkin_Implementation(int32 NewIdx)
{
	Multicast_ApplyCharacterSkin(NewIdx);	
}

void AMainMenuPawn::Multicast_ApplyCharacterSkin_Implementation(int32 NewIdx)
{
	CurrentSkinIdx = NewIdx;
	CharacterSkin = GetSkinByIndex(NewIdx);
	SetCharacterMaterials();  

	// 선택한 스킨을 GameInstance에 저장
	if (UGI* GameInstance = Cast<UGI>(GetGameInstance()))
	{
		GameInstance->SkinIdx = NewIdx;
	}	
}

void AMainMenuPawn::UpdateSkinPreview(int32 NewIdx)
{
	CharacterSkin = GetSkinByIndex(NewIdx);
	SetCharacterMaterials();  
}




