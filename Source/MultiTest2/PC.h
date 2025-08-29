// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST2_API APC : public APlayerController
{
	GENERATED_BODY()

protected:	
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaTime) override;

	/* 위젯 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UStartAlert> StartAlertWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UTimer> TimerWidgetClass;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UResult> ResultWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UCrossHair> CrossWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UMinimap> MinimapWidgetClass;
	
	UPROPERTY()
	UTimer* TimerWidget;
	UPROPERTY()
	UStartAlert* StartAlertWidget;
	UPROPERTY()
	UResult* ResultWidget;
	UPROPERTY()
	UCrossHair* CrossWidget;
	UPROPERTY()
	UMinimap* MinimapWidget;

	UFUNCTION()
	void CreateStartWidget();
	UFUNCTION()
	void CreateTimerWidget();
	UFUNCTION()
	void CreateCrossWidget();
	UFUNCTION()
	void CreateResultWidget();
	UFUNCTION()
	void CreateMinimapWidget();
	
	UFUNCTION(Client, Reliable)
	void RemoveCrossWidget();
	UFUNCTION(Client, Reliable)
	void RemoveTimerWidget();
	UFUNCTION(Client, Reliable)
	void RemoveMinimapWidget();
	
	UFUNCTION()
	void ShowStartWidgetAndStartAnim();
	UFUNCTION(Client, Reliable)
	void Client_ShowStartWidgetAndStartAnim();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowStartWidgetAndStartAnim();
	UFUNCTION(BlueprintCallable)
	void ToggleMinimap();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowMinimap();
	
	UFUNCTION()
	void UpdateTimerWidget();
	
	UFUNCTION(Server, Reliable)
	void Server_NotifyClientReady();

	UFUNCTION(Client, Reliable)
	void Client_ShowResultUI();
	UFUNCTION(Client, Reliable)
	void Client_UpdateResultUI(float InSmokerRate, float InNonSmokerRate);

	UFUNCTION(Server, Reliable)
	void Server_NotifyResultWidgetClosed();
	UFUNCTION(Client, Reliable)
	void Client_NotifyResultWidgetClosed();
	
	/* 변수 */
	UPROPERTY()
	bool bMinimapVisible = false;
	UPROPERTY()
	bool bStartAlertShown = false;
	UPROPERTY()
	bool bIsReadyToStart = false;
	
	UPROPERTY()
	UAudioComponent* BGMComponent;
	UPROPERTY(EditAnywhere, Category = "BGM")
	TArray<USoundBase*> BGMSoundList;

	// 클라이언트 전용 재생 함수
	UFUNCTION(Client, Reliable)
	void Client_StartRandomBGM();
	UFUNCTION()
	void PlayRandomBGM();
	UFUNCTION(Client, Reliable)
	void Client_StopBGM();

	UFUNCTION()
	void UpdatePaintUI();
};


