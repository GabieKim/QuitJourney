#pragma once

#include "CoreMinimal.h"
#include "StChatColor.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct MULTITEST2_API FStChatColor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Key;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor Color;
};
