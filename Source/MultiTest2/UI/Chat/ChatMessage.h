// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiTest2/ChatSystem/Structure/StChatMessage.h"
#include "ChatMessage.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST2_API UChatMessage : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativePreConstruct() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta =(BindWidget), Category="Msg")
	class UTextBlock* TB_PlayerName;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta =(BindWidget), Category="Msg")
	class UTextBlock* TB_Message;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta =(BindWidget), Category="Msg")
	class UTextBlock* TB_Time;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	FStChatMessage MessageVariable;
	
};
