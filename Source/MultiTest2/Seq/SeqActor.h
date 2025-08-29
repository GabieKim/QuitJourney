// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MultiTest2/MyCharacter.h"

class AMyCharacter;
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../CharacterSkins.h"
#include "SeqActor.generated.h"

UCLASS()
class MULTITEST2_API ASeqActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASeqActor();
	USkeletalMeshComponent* GetSkeletalMeshComponent() const { return MySkeletalMesh.Get(); }	

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterData)
	EPlayerType PlayerType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<USkeletalMeshComponent> MySkeletalMesh;

	struct FCharacterSkins* CharacterSkins;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData, meta = (AllowPrivateAccess = "true"))
	class UDataTable* CharacterSkinDataTable;
	UPROPERTY()
	int32 MySkinIdx =-1;

	UFUNCTION()
	void SetSkinIdx(int32 Value);
	FCharacterSkins* GetSkinByIndex(int32 Index) const;
	
public:	
	virtual void Tick(float DeltaTime) override;

};
