// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Structure/StUserChatSettings.h"
#include "ChatSystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChatOpened);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChatClosed);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTITEST2_API UChatSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UChatSystemComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UMainChatSystem> MainChatWidgetRef;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class APlayerController> PlayerController;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class AGM> GameModeInstance;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chat Settings")
	FStUserChatSettings PlayerChatSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	class UInputMappingContext* IMC_Chat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsOpen = false;

	// BP에서 바인딩 가능한 델리게이트 2개
	UPROPERTY(BlueprintAssignable, Category = "Chat")
	FOnChatOpened OpenChatBoxEvent;
	UPROPERTY(BlueprintAssignable, Category = "Chat")
	FOnChatClosed CloseChatBoxEvent;

	// 위젯
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UMainChatSystem> MainChatSystemWidget;	
	UPROPERTY()
	UMainChatSystem* MainChatSystemRef;
	
	// ----------------------함수------------------------------------ //
	UFUNCTION(Category="ChatStart")
	void StartChatSystem(APlayerController* PC);
	UFUNCTION(Category="ChatStart")
	void SetChatIMC(APlayerController* PC);
	UFUNCTION(Category="ChatStart")
	void CreateMainChatSystemUI();
	UFUNCTION(Category="ChatStart")
	void SetCloseOpenEvent();

	
	UFUNCTION()
	void ToggleChat(APlayerController* PC);
	void OpenChat(APlayerController* PC);
	void CloseChat(APlayerController* PC);
	void AddChatMessage(const FString&UserId, const FString& UserName, const FLinearColor& Color);
};
