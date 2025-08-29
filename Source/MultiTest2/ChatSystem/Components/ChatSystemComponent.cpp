// Fill out your copyright notice in the Description page of Project Settings.


#include "../Components/ChatSystemComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "MultiTest2/GM.h"
#include "MultiTest2/UI/Chat/MainChatSystem.h"

class UEnhancedInputLocalPlayerSubsystem;

UChatSystemComponent::UChatSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	
}


void UChatSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// Init : 게임모드 형변환하기
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (!GameMode) return;

	AGM* MyGM = Cast<AGM>(GameMode);
	if (!MyGM) return;
	
	
}

void UChatSystemComponent::StartChatSystem(APlayerController* PC)
{
	
}

void UChatSystemComponent::SetChatIMC(APlayerController* PC)
{
	// if (!PlayerController) return;
	//
	// if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	// {
	// 	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
	// 		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	// 	{
	// 		Subsystem->AddMappingContext(IMC_Chat, 1);
	// 	}
	// }	
}

void UChatSystemComponent::CreateMainChatSystemUI()
{
	// if (!MainChatSystemWidget && MainChatSystemRef)
	// {
	// 	MainChatSystemRef = CreateWidget<UMainChatSystem>(this, MainChatSystemWidget);
	// 	if (MainChatSystemRef)
	// 	{
	// 		MainChatSystemRef->AddToViewport();
	// 		MainChatSystemRef->SetVisibility(ESlateVisibility::Hidden);
	// 	}
	// }	
}

void UChatSystemComponent::SetCloseOpenEvent()
{
}

void UChatSystemComponent::ToggleChat(APlayerController* PC)
{
}

void UChatSystemComponent::OpenChat(APlayerController* PC)
{
	// bIsOpen = true;
	// if (!PC) return;	
	// PC->bShowMouseCursor = true;
	//
	// FInputModeGameAndUI InputMode;
	// InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	// InputMode.SetHideCursorDuringCapture(false);
	// //InputMode.SetWidgetToFocus(ETB_ChatInput->TakeWidget());
	//
	// PC->SetInputMode(InputMode);
	
	
}

void UChatSystemComponent::CloseChat(APlayerController* PC)
{
	// bIsOpen = false;
	// if (!PC) return;
	// PC->bShowMouseCursor = false;
	// FInputModeGameOnly InputMode;
	// PC->SetInputMode(InputMode);	
}

void UChatSystemComponent::AddChatMessage(const FString& UserId, const FString& UserName, const FLinearColor& Color)
{
}





