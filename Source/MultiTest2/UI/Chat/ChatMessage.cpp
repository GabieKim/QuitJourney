// Fill out your copyright notice in the Description page of Project Settings.


#include "../Chat/ChatMessage.h"
#include "Components/TextBlock.h"
#include "MultiTest2/ChatSystem/Structure/StChatMessage.h"

void UChatMessage::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	// if (TB_PlayerName)
	// {
	// 	TB_PlayerName->SetText(FText::FromString(MessageVariable.UserName));
	// 	TB_PlayerName->SetColorAndOpacity(FSlateColor(MessageVariable.UserColor));
	// }
	//
	// if (TB_Message)
	// {
	// 	TB_Message->SetText(MessageVariable.Msg);
	// }
	//
	// if (TB_Time)
	// {
	// 	TB_Time->SetText(FText::FromString(MessageVariable.TimeStamp));
	// }
}
