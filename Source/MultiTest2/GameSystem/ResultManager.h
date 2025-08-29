// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResultManager.generated.h"

UCLASS()
class MULTITEST2_API AResultManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AResultManager();

public:
	/* 컴포넌트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultSystem")
	USceneComponent* Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultSystem")
	USceneCaptureComponent2D* TopDownCapture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultSystem")
	UTextureRenderTarget2D* CapturedResultRT;

	/* 비긴플레이에 들어가는 함수들 */
	virtual void BeginPlay() override;	
	UFUNCTION(Category = "BeginPlayFunc")
	void InitializeSceneCapture();
	UFUNCTION(Category = "BeginPlayFunc")
	void HidePlayersFromCapture();

	UFUNCTION(Category = "BeginPlayFunc")
	void BindResultDelegates();

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void StartLerp();

	/* 계산 결과 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ResultSystem")
	float SmokerRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ResultSystem")
	float NonSmokerRate;

	/* UI 상태 관리용 내부 변수 */
	bool bIsSmokerScoreLerping = false;
	bool bIsNonSmokerScoreLerping = false;

	/* 컬러 기준값 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuitJourney|Paint")
	FLinearColor SmokerPaintColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuitJourney|Paint")
	FLinearColor NonSmokerPaintColor;

	/* 함수 */	
	/*스닙샷패인트맵에 들어가는 함수 모음*/
	UFUNCTION(BlueprintCallable, Category = "SnapshotPaintMap")
	void SnapshotPaintMap();	
	UFUNCTION(BlueprintCallable, Category = "SnapshotPaintMap")
	void CapturePaintMap();
	UFUNCTION(BlueprintCallable, Category = "SnapshotPaintMap")
	bool ReadCapturedPixels(TArray<FLinearColor>& OutPixels);
	UFUNCTION(BlueprintCallable, Category = "SnapshotPaintMap")
	void CountTeamPixels(const TArray<FLinearColor>& Pixels, int32& OutSmoker, int32& OutNonSmoker);
	UFUNCTION(BlueprintCallable, Category = "SnapshotPaintMap")
	void CalculatePaintRates(int32 SmokerCount, int32 NonSmokerCount);
	UFUNCTION(BlueprintCallable, Category = "SnapshotPaintMap")
	void ApplyPaintResults();
	UFUNCTION(BlueprintCallable, Category = "SnapshotPaintMap")
	bool AreColorsSimilar(const FLinearColor& A, const FLinearColor& B, float Tolerance);
	
	//-------------------------------------------------------------
	// 승리팀 판정 (스모커 승리 여부)
	UFUNCTION(BlueprintCallable, Category = "MySetting")
	bool DidSmokerWin();
	//-------------------------------------------------------------

	
private:
	UPROPERTY(VisibleAnywhere, Category = "MySetting")
	class APC* MyPC;
	UPROPERTY(VisibleAnywhere, Category = "MySetting")
	class AMainHUD* MyHud;

	
};
