#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterSkins.generated.h"

USTRUCT(BluePrintType)
struct MULTITEST2_API FCharacterSkins : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Mat0_Hair;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Mat1_Fabric;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Mat2_Scarf;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Mat3_SKin;
	
	TArray<UMaterialInterface*> GetMaterialArray() const
	{
		return { Mat0_Hair, Mat1_Fabric, Mat2_Scarf, Mat3_SKin };
	}	
};
