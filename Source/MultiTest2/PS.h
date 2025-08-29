// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyCharacter.h"
#include "PS.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnXpChanged, int32, NewXp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterLevelUp, int32, NewLevelXp);

UCLASS()
class MULTITEST2_API APS : public APlayerState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Experience", ReplicatedUsing="OnRep_Xp")
	int Xp = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Experience", ReplicatedUsing="OnRep_CharacterLevelUp")
	int CharacterLevel = 1;

	UFUNCTION()
	void OnRep_Xp(int32 OldValue)const;
	UFUNCTION()
	void OnRep_CharacterLevelUp(int32 OldValue)const;
	// UFUNCTION()
	// void OnRep_PlayerTypeChanged(EPlayerType OldPlayerType)const;

	UPROPERTY()
	int32 RetryCount = 0;
	int32 MaxRetryCount = 10;
	
public:
	UFUNCTION(BlueprintCallable, Category="Experience")
	void AddXp(int32 Value);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnXpChanged OnXpChanged;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCharacterLevelUp OnCharacterLevelUp;

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Type)
	EPlayerType PlayerType = EPlayerType::None;
	
	UFUNCTION()
	void OnRep_Type();
	

};







