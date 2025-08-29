// Fill out your copyright notice in the Description page of Project Settings.


#include "GI.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"

void UGI::SetPlayerType(EPlayerType NewType)
{
	PlayerType = NewType;
}

EPlayerType UGI::GetPlayerType() const
{
	return PlayerType;
}

void UGI::PlayBGM(int32 Index)
{
	if (!BGMList.IsValidIndex(Index)) return;

	if (CurrentBGMComponent && CurrentBGMComponent->IsPlaying())
	{
		CurrentBGMComponent->Stop();
	}

	USoundBase* SelectedBGM = BGMList[Index];
	if (SelectedBGM)
	{
		CurrentBGMComponent = UGameplayStatics::CreateSound2D
		(this, SelectedBGM, 1.0f, 1.0f, 0.0f, nullptr, true);

		if (CurrentBGMComponent)
		{
			CurrentBGMComponent->bIsUISound = true;
			CurrentBGMComponent->bAutoActivate = false;
			CurrentBGMComponent->Play();
		}
	}
}
