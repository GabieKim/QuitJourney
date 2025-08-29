#pragma once

#include "CoreMinimal.h"
#include "StUserChatSettings.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct MULTITEST2_API FStUserChatSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor Color;
};
