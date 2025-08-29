// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinionSpawner.generated.h"

UCLASS()
class MULTITEST2_API AMinionSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AMinionSpawner();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnSystem")
	TArray<TSubclassOf<class AMinion>> SpawnableMinions;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnSystem")
	float SpawnDelay = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnSystem")
	int32 NumMinionsAtStart = 5;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void Spawn();
	
public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnSystem",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> SpawnArea;

	UPROPERTY()
	FTimerHandle SpawnTimerHandle;
	
	
};
