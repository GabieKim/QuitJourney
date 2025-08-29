// Fill out your copyright notice in the Description page of Project Settings.


#include "../Paint/PaintableActor.h"

// #include "SWarningOrErrorBox.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"

APaintableActor::APaintableActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);
}

/*
 * 기능 : 초기 렌더타겟 생성 + 머티리얼 연결
 */
void APaintableActor::BeginPlay()
{
	Super::BeginPlay();

	// CRT_DamageMask 초기화
	CRT_CalcDamageMask = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 1024, 1024, RTF_RGBA32f);
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), CRT_CalcDamageMask, FLinearColor(2, 2, 2, 1));
	// 시각용 RT 생성
	CRT_VisualDamageMask = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 1024, 1024, RTF_RGBA32f);
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), CRT_VisualDamageMask, FLinearColor(0, 0, 0, 1));
	
	// 메시 UV 펼치고 머티리얼 설정
	Unwrap();
}

void APaintableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/** 
 * 기능 : 특정 UV 위치에 브러시 머티리얼로 칠함
 * @param bIsTeamSmoke - 스모커팀인지 여부
 * @param DrawLocation - UV 기준으로 0~1 사이의 브러시 위치
 */
void APaintableActor::BePainted(bool bIsTeamSmoke, FVector2D DrawLocation)
{
	//UE_LOG(LogTemp, Warning, TEXT("BePainted: %s at UV: %s"), bIsTeamSmoke ? TEXT("Smoker") : TEXT("NonSmoker"), *DrawLocation.ToString());
	// 해당 팀에 맞는 브러시 머티리얼 선택
	UMaterialInterface* BrushTexture = bIsTeamSmoke ? M_Brush_TeamSmoke : M_Brush_TeamNonSmoke;
	if (!BrushTexture) return;

	UCanvas* Canvas = nullptr;
	FVector2D Size;
	FDrawToRenderTargetContext Context;

	// RenderTarget에 직접 그림 그리기 시작
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), CRT_VisualDamageMask, Canvas, Size, Context);
	if (!Canvas) return;

	// 브러시 위치 계산 (중앙 정렬)
	FVector2D BrushLocation = (Size * DrawLocation) - FVector2D(BrushSize * 0.5f);

	// 해당 위치에 머티리얼을 사용해 색칠
	Canvas->K2_DrawMaterial(BrushTexture, BrushLocation, FVector2D(BrushSize), FVector2D(0, 0), FVector2D(1, 1));
	
	// 캔버스 그리기 종료
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
}

/**
 * 기능 1: RenderTarget에 브러시를 찍어 페인팅 효과를 적용
 * 기능 2: 팀(흡연/비흡연)에 따라 계산용 및 시각용 RenderTarget에 각각 칠함
 * @param bIsTeamSmoke     팀 구분 (true: 흡연자팀 / false: 비흡연자팀)
 * @param DrawLocation     UV 좌표계 기준의 칠할 위치 (0~1 범위)
 * @param TeamColor        팀 색상 (계산용 마스크에 사용됨)
 */
void APaintableActor::BePainted(bool bIsTeamSmoke, FVector2D DrawLocation,FLinearColor TeamColor)
{
	// ───── 계산용 RenderTarget에 브러시 찍기 ─────
	if (CRT_CalcDamageMask)
	{
		// 팀에 따라 계산용 브러시 머티리얼 선택
		UMaterialInterface* CalcBrushMat = bIsTeamSmoke ? M_CalcBrush_TeamSmoke : M_CalcBrush_TeamNonSmoke;
		if (CalcBrushMat)
		{
			// 동적 머티리얼 인스턴스를 생성하고 색상 파라미터 설정
			UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(CalcBrushMat, this);
			DynMat->SetVectorParameterValue(FName("BrushColor"), TeamColor);

			// RenderTarget에 그리기
			DrawToRenderTarget(CRT_CalcDamageMask, DynMat, DrawLocation);
		}
	}

	// ───── 시각용 RenderTarget에 브러시 찍기 ─────
	if (CRT_VisualDamageMask)
	{
		// 팀에 따라 시각용 브러시 머티리얼 선택
		UMaterialInterface* VisualBrushMat = bIsTeamSmoke ? M_Brush_TeamSmoke : M_Brush_TeamNonSmoke;
		if (VisualBrushMat)
		{
			// RenderTarget에 그리기
			DrawToRenderTarget(CRT_VisualDamageMask, VisualBrushMat, DrawLocation);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO VisualDamageMask"));
	}
}

/**
 * 기능 : 주어진 RenderTarget에 머티리얼을 사용하여 지정된 위치에 브러시를 찍기
 * @param Target        대상 RenderTarget
 * @param Material      사용할 머티리얼 (동적 or 정적)
 * @param DrawLocation  UV 좌표계 기준의 위치 (0~1 범위)
 */
void APaintableActor::DrawToRenderTarget(UTextureRenderTarget2D* Target, UMaterialInterface* Material,
	const FVector2D& DrawLocation)
{
	if (!Target || !Material) return;

	// 캔버스 생성 및 렌더 타겟 바인딩
	UCanvas* Canvas = nullptr;
	FVector2D Size;
	FDrawToRenderTargetContext Context;

	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), Target, Canvas, Size, Context);
	if (!Canvas)
	{
		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
		return;
	}

	// UV 좌표계 기준으로 브러시 위치 계산 (중앙 정렬)
	const FVector2D BrushLocation = (Size * DrawLocation) - FVector2D(BrushSize * 0.5f);

	// 캔버스에 머티리얼을 직접 그리기
	Canvas->K2_DrawMaterial(Material,
		BrushLocation,
		FVector2D(BrushSize),
		FVector2D(0, 0),
		FVector2D(1, 1));

	// 캔버스 렌더링 종료
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
}

void APaintableActor::Unwrap()
{
	// 1. 렌더 타겟 초기화
	if (!CRT_CalcDamageMask)
	{
		CRT_CalcDamageMask = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 1024, 1024, RTF_RGBA32f);
		UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), CRT_CalcDamageMask, FLinearColor::Black);
	}

	if (!CRT_VisualDamageMask)
	{
		CRT_VisualDamageMask = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 1024, 1024, RTF_RGBA32f);
		UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), CRT_VisualDamageMask, FLinearColor::Black);
	}

	// 2. 메시 머티리얼을 동적 인스턴스로 교체하고, 두 개의 RT 연결
	const int32 MaterialCount = StaticMesh->GetMaterials().Num();
	OriginMatArray.Empty();
	TempMatArray.Empty();

	for (int32 i = 0; i < MaterialCount; ++i)
	{
		UMaterialInterface* OriginalMat = StaticMesh->GetMaterial(i);
		if (!OriginalMat) continue;

		OriginMatArray.Add(OriginalMat);

		UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(OriginalMat, this);
		if (DynMat)
		{
			// 슬롯마다 다르게 처리
			if (i == 0) // 사용자 시각용 ← 바꿔주자!
			{
				DynMat->SetTextureParameterValue(FName("VisualDamageTexture"), CRT_VisualDamageMask);
			}
			else if (i == 1) // 계산용
			{
			//	DynMat->SetTextureParameterValue(FName("DamageTexture"), CRT_CalcDamageMask);
			}

			TempMatArray.Add(DynMat);
			StaticMesh->SetMaterial(i, DynMat);
		}
	}
}
