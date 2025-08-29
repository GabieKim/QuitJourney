// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyCharacter.h"
#include "Sound/SoundBase.h"
#include "GI.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST2_API UGI : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TMap<EPlayerType, int32> SkinIdxMap;
	
	UPROPERTY(BlueprintReadWrite)
	int32 SkinIdx = -1;
	
	UPROPERTY(BlueprintReadWrite, Category = "Character")
	EPlayerType PlayerType;

	void SetPlayerType(EPlayerType NewType);
	EPlayerType GetPlayerType() const;
	
	// BGM 사운드만 저장할 배열
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM")
	TArray<USoundBase*> BGMList;

	// BGM 재생용 함수 
	UFUNCTION(BlueprintCallable, Category = "BGM")
	void PlayBGM(int32 Index);

private:
	// 실행 중 재생된 BGM 추적용
	UPROPERTY()
	UAudioComponent* CurrentBGMComponent;
};
