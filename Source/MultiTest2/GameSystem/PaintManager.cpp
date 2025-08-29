// Fill out your copyright notice in the Description page of Project Settings.


#include "../GameSystem/PaintManager.h"

#include "EngineUtils.h"
#include "ResultManager.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/World.h"
#include "Engine/TextureRenderTarget2D.h"


class AResultManager;

APaintManager::APaintManager()
{
	PrimaryActorTick.bCanEverTick = false;
	

}

void APaintManager::BeginPlay()
{
	Super::BeginPlay();
	
	SetColorsFromMyCharacter();
}

void APaintManager::SetColorsFromMyCharacter()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;
	
	AMyCharacter* MyCharacter = Cast<AMyCharacter>(PC->GetPawn());
	if (MyCharacter)
	{
		SmokerColor = MyCharacter->SmokerColor;
		NonsmokerColor = MyCharacter->NonSmokerColor;
	}
}

void APaintManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

ESurfaceType APaintManager::GetSurfaceTypeAtLocation(const FVector& WorldLocation) const
{
	if (!PaintRenderTarget || !PaintOriginReference)
	{
		UE_LOG(LogTemp, Error, TEXT("APaintManager::GetSurfaceTypeAtLocation1"));
		return ESurfaceType::Default;
	}

	// 1. 월드 위치 → UV 변환
	FVector2D UV = ConvertWorldToUV(WorldLocation);
	if (UV.X < 0.f || UV.X > 1.f || UV.Y < 0.f || UV.Y > 1.f)
	{
		UE_LOG(LogTemp, Error, TEXT("APaintManager::GetSurfaceTypeAtLocation2"));
		return ESurfaceType::Default; // 범위 밖이면 기본
	}

	// 2. 픽셀 좌표 계산
	int32 X = UV.X * PaintRenderTarget->SizeX;
	int32 Y = UV.Y * PaintRenderTarget->SizeY;
	int32 Index = Y * PaintRenderTarget->SizeX + X;
	UE_LOG(LogTemp, Error, TEXT("APaintManager::GetSurfaceTypeAtLocation3"));
	
	// 3. RenderTarget에서 픽셀 색 읽기
	FTextureRenderTargetResource* Resource = PaintRenderTarget->GameThread_GetRenderTargetResource();
	if (!Resource) return ESurfaceType::Default;

	TArray<FColor> PixelData;
	Resource->ReadPixels(PixelData);

	if (!PixelData.IsValidIndex(Index))
	{
		UE_LOG(LogTemp, Error, TEXT("APaintManager::GetSurfaceTypeAtLocation4"));
		return ESurfaceType::Default;
	}

	FLinearColor PixelColor = PixelData[Index].ReinterpretAsLinear();

	// 4. 색으로 판단
	if (PixelColor.Equals(SmokerColor, 0.1f))
	{
		return ESurfaceType::SmokerPainted;
	}
	else if (PixelColor.Equals(NonsmokerColor, 0.1f))
	{
		return ESurfaceType::NonsmokerPainted;
	}

	return ESurfaceType::Default;
}

/*
 * 기능 : 	월드 좌표를 RenderTarget 기준 UV로 변환
 */
FVector2D APaintManager::ConvertWorldToUV(const FVector& WorldPos) const
{
	if (!PaintOriginReference)
	{
		UE_LOG(LogTemp, Error, TEXT("APaintManager::ConvertWorldToUV 1"));
		return FVector2D(0.0f, 0.0f);		
	}
	FVector LocalPos = PaintOriginReference->GetActorTransform().InverseTransformPosition(WorldPos);
	float U = (LocalPos.X + WorldPaintSize.X * 0.5f) / WorldPaintSize.X;
	float V = (LocalPos.Y + WorldPaintSize.Y * 0.5f) / WorldPaintSize.Y;
	
	return FVector2D(U, V);
}


