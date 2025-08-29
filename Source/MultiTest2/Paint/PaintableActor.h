// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaintableActor.generated.h"

UCLASS()
class MULTITEST2_API APaintableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APaintableActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	void BePainted(bool bIsTeamSmoke, FVector2D DrawLocation);
	void BePainted(bool bIsTeamSmoke, FVector2D DrawLocation, FLinearColor TeamColor);
	void DrawToRenderTarget(UTextureRenderTarget2D* Target, UMaterialInterface* Material, const FVector2D& DrawLocation);
	
	// 실제 칠해진 정보가 저장되는 텍스처
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Paintable|RenderTarget")
	UTextureRenderTarget2D* CRT_CalcDamageMask;	
	// 비주얼적으로 칠해진 정보가 저장되는 텍스처
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Paintable|RenderTarget")
	UTextureRenderTarget2D* CRT_VisualDamageMask;
	
	/** 각 팀의 브러시 컬러 지정용 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PixelColor")
	FColor SmokerPaintColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PixelColor")
	FColor NonSmokerPaintColor;
	UPROPERTY(EditAnywhere, Category = "Paintable|Debug")
	bool bSpawnVisualDecal = true;
	
	/** 컴포넌트 */
	UPROPERTY(VisibleAnywhere, Category = "Paintable|Components")
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, Category = "Paintable|Components")
	UStaticMeshComponent* StaticMesh;
	
private:
	// 메시 UV를 펼치고 머티리얼 연결
	void Unwrap();



	/** 브러시 머티리얼 */
	UPROPERTY(EditAnywhere, Category = "Paintable|Brush")
	UMaterialInterface* M_Brush_TeamSmoke;
	UPROPERTY(EditAnywhere, Category = "Paintable|Brush")
	UMaterialInterface* M_Brush_TeamNonSmoke;

	/** RenderTarget에 실제 칠할 때 사용하는 계산용 브러시 언릿 머티리얼 */
	UPROPERTY(EditAnywhere, Category = "Paintable|Brush")
	UMaterialInterface* M_CalcBrush_TeamSmoke;
	UPROPERTY(EditAnywhere, Category = "Paintable|Brush")
	UMaterialInterface* M_CalcBrush_TeamNonSmoke;

	/** 언랩용 머티리얼 */
	UPROPERTY(EditAnywhere, Category = "Paintable|Materials")
	UMaterialInterface* M_Unwrap;
	UPROPERTY(EditAnywhere, Category = "Paintable|Materials")
	UMaterialInterface* M_PaintTexture;

	UPROPERTY(Transient)
	TArray<UMaterialInterface*> OriginMatArray;
	UPROPERTY(Transient)
	TArray<UMaterialInstanceDynamic*> TempMatArray;
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* MI_PaintTexture;

	UPROPERTY(EditAnywhere, Category = "Paintable|RenderTarget")
	UTextureRenderTarget2D* CRT_UnWrap;

	/** 브러시 크기 */
	UPROPERTY(EditAnywhere, Category = "Paintable|Brush")
	float BrushSize = 300.f;
};




