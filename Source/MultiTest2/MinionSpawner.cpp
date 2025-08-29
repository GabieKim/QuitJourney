// Fill out your copyright notice in the Description page of Project Settings.


#include "MinionSpawner.h"
#include "Minion.h"
#include "Components/BoxComponent.h"

AMinionSpawner::AMinionSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnArea = CreateDefaultSubobject<UBoxComponent>("SpawnArea");
	SpawnArea->SetupAttachment(RootComponent);
	SpawnArea->SetBoxExtent(FVector(1000.f, 1000.f, 100.f));
}

void AMinionSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnableMinions.IsEmpty())
	{
		return;
	}
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (int32 i = 0; i < NumMinionsAtStart; i++)
	{
		Spawn();
	}

	// 스폰딜레이 시간 간격으로 Spawn() 호출
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this
		,&AMinionSpawner::Spawn, SpawnDelay, true, SpawnDelay);
}

void AMinionSpawner::Spawn()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding; 
	//충돌X -> 정상 스폰
	//충돌O -> 가능 시 위치 조정해서 스폰 else if 스폰 X

	auto Minion = SpawnableMinions[FMath::RandRange(0, SpawnableMinions.Num() - 1)];

	// 랜덤한 Rotation, Location 만들어줌
	const auto Rotation = FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f);
	const auto Location = SpawnArea->GetComponentLocation() +
		FVector(
			FMath::RandRange(-SpawnArea->GetScaledBoxExtent().X,
				SpawnArea->GetScaledBoxExtent().X), 
			FMath::RandRange(-SpawnArea->GetScaledBoxExtent().Y,
				SpawnArea->GetScaledBoxExtent().Y), 
			0.0f);

	GetWorld()->SpawnActor<AMinion> (Minion, Location, Rotation, SpawnParams);
}

void AMinionSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

