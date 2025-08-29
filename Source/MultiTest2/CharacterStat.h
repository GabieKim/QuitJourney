#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterStat.generated.h"

USTRUCT(BlueprintType)
struct MULTITEST2_API FCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float WalkSpeed = 200.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SprintSpeed = 200.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DamageMultiplier = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 NextLevelXp = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float StealMultiplier = 1.0f;	
};
