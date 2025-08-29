// Fill out your copyright notice in the Description page of Project Settings.


#include "PS.h"

#include "CharacterStat.h"
#include "MyCharacter.h"
#include "Elements/Columns/TypedElementAlertColumns.h"
#include "Net/UnrealNetwork.h"

void APS::OnRep_Xp(int32 OldValue) const
{
	OnXpChanged.Broadcast(Xp);
}

void APS::OnRep_CharacterLevelUp(int32 OldValue) const
{
	OnXpChanged.Broadcast(CharacterLevel);
}

// void APS::OnRep_PlayerTypeChanged(EPlayerType OldPlayerType) const
// {
// 	//On
// }

void APS::AddXp(int32 Value)
{
	Xp += Value;
	OnXpChanged.Broadcast(Xp);

	GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Yellow,
		FString::Printf(TEXT("Total Xp : %d"), Value));

	if (const auto Character = Cast<AMyCharacter>(GetPawn()))
	{
		if (Character->GetCharacterStat()->NextLevelXp < Xp)
		{
			GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, TEXT("LevelUp!~"));

			CharacterLevel++;
			Character->UpdateCharacterStat(CharacterLevel);
			OnCharacterLevelUp.Broadcast(CharacterLevel);			
		}
	}
	
}

void APS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APS, Xp, COND_OwnerOnly); // 클라에게만 복제되도록 오너온리
	DOREPLIFETIME_CONDITION(APS, CharacterLevel, COND_OwnerOnly);
	DOREPLIFETIME(APS, PlayerType);
}

void APS::OnRep_Type()
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		if (RetryCount < MaxRetryCount)
		{
			RetryCount++;
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APS::OnRep_Type, 0.5f, false); // 5초는 너무 김
		}
		return;
	}	
}
