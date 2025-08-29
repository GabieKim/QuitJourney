// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StChatMessage.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct MULTITEST2_API FStChatMessage
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 UserId;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString UserName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Msg;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString TimeStamp;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor UserColor;
};
