// Fill out your copyright notice in the Description page of Project Settings.


#include "../GameSystem/ResultManager.h"

#include "EngineUtils.h"
#include "../PC.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/TextBlock.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "MultiTest2/MyCharacter.h"
#include "MultiTest2/Paint/PaintableActor.h"
#include "MultiTest2/UI/MainHUD.h"
#include "MultiTest2/UI/Result.h"
#include "../GS.h"

AResultManager::AResultManager()
{
	//PrimaryActorTick.bCanEverTick = true;
	
	Root =CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	TopDownCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("TopDowCapture"));
	TopDownCapture->SetupAttachment(RootComponent);
	TopDownCapture->SetRelativeLocation(FVector(0.f, 0.f, 0.f));  
	TopDownCapture->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); // Pitch, Yaw, Roll
	TopDownCapture->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
}

void AResultManager::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeSceneCapture();
	//HidePlayersFromCapture();
	BindResultDelegates();
}

void AResultManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



void AResultManager::StartLerp()
{
	bIsSmokerScoreLerping = true;
	bIsNonSmokerScoreLerping = true;
	SetActorTickEnabled(true);
}

/*
 * 기능 : 신캡쳐 초기화
 */
void AResultManager::InitializeSceneCapture()
{
	TopDownCapture->TextureTarget = CapturedResultRT;
}

/*
 * 기능  : 레벨에 있는 플레이어 숨겨주기
 * why? : 결과 찍을 때 플레이어 있으면 정확하게 어려우니까 -> 근데 일단 한마리만 감춰짐! 무기도 감춰야햄
 */
void AResultManager::HidePlayersFromCapture()
{
	if (!IsNetMode(NM_Client))
	{
		// 클라이언트가 아니면 스킵
		return;
	}
	for (TActorIterator<AMyCharacter> Player(GetWorld()); Player; ++Player)
	{
		//TopDownCapture->HideActorComponents(*Player, true);
		if (Player->GetMesh())
		{
			TopDownCapture->HideComponent(Player->GetMesh());
			//Player->HideWeapon_Implementation(Player->PlayerType);			
		}
	}	
}

void AResultManager::BindResultDelegates()
{
	if (MyHud)
	{
		MyHud->OnCalcPaintRequest.AddDynamic(this,&AResultManager::SnapshotPaintMap);
		MyHud->OnCapturedRequest.AddDynamic(this,&AResultManager::CapturePaintMap);
	}
}

void AResultManager::SnapshotPaintMap()
{
	APaintableActor* Paintable = Cast<APaintableActor>(
		UGameplayStatics::GetActorOfClass(GetWorld(), APaintableActor::StaticClass()));

	if (!Paintable || !Paintable->CRT_CalcDamageMask)
	{
		UE_LOG(LogTemp, Warning, TEXT("PaintableActor를 찾을 수 없거나 CRT_DamageMask가 없음!"));
		return;
	}

	FTextureRenderTarget2DResource* RTResource =
		static_cast<FTextureRenderTarget2DResource*>(Paintable->CRT_CalcDamageMask->GetResource());

	TArray<FLinearColor> Pixels;
	if (!RTResource || !RTResource->ReadLinearColorPixels(Pixels))
	{
		UE_LOG(LogTemp, Warning, TEXT("RenderTarget에서 픽셀 읽기 실패"));
		return;
	}

	int32 SmokerCount = 0;
	int32 NonSmokerCount = 0;

	CountTeamPixels(Pixels, SmokerCount, NonSmokerCount); 
	CalculatePaintRates(SmokerCount, NonSmokerCount);
	ApplyPaintResults();
}

void AResultManager::CapturePaintMap()
{
	TopDownCapture->CaptureScene();
}

bool AResultManager::ReadCapturedPixels(TArray<FLinearColor>& OutPixels)
{
	FTextureRenderTarget2DResource* Resource = static_cast<FTextureRenderTarget2DResource*>(CapturedResultRT->GetResource());
	return Resource && Resource->ReadLinearColorPixels(OutPixels);
}
bool AResultManager::AreColorsSimilar(const FLinearColor& A, const FLinearColor& B, float Tolerance)
{
	return FMath::Abs(A.R - B.R) < Tolerance &&
		   FMath::Abs(A.G - B.G) < Tolerance &&
		   FMath::Abs(A.B - B.B) < Tolerance;
}

void AResultManager::CountTeamPixels(const TArray<FLinearColor>& Pixels, int32& OutSmoker, int32& OutNonSmoker)
{
	OutSmoker = 0;
	OutNonSmoker = 0;

	int32 PrintLimit = 100;

	APaintableActor* Paintable = Cast<APaintableActor>(
	UGameplayStatics::GetActorOfClass(GetWorld(), APaintableActor::StaticClass()));

	NonSmokerPaintColor = Paintable->NonSmokerPaintColor; 
	SmokerPaintColor= Paintable->SmokerPaintColor;
	
	for (const FLinearColor& Color  : Pixels)
	{
		if (AreColorsSimilar(Color, NonSmokerPaintColor, 0.2f))
		{
			OutNonSmoker++;
		}
		if (AreColorsSimilar(Color, SmokerPaintColor, 0.2f)) 
		{
			OutSmoker++;
		}
		
	}
	UE_LOG(LogTemp, Warning, TEXT("Smoker 픽셀 수: %d"), OutSmoker);
	UE_LOG(LogTemp, Warning, TEXT("NonSmoker 픽셀 수: %d"), OutNonSmoker);	
}

/*
 * 기능 : 매개변수로 받은 SmokerCount와 NonSmokerCount로 각각의 페인트칠 비율 정함
 */
void AResultManager::CalculatePaintRates(int32 SmokerCount, int32 NonSmokerCount)
{
	int32 TotalCount = SmokerCount + NonSmokerCount;

	if (TotalCount > 0)
	{
		SmokerRate = static_cast<float>(SmokerCount) / TotalCount;
		NonSmokerRate = static_cast<float>(NonSmokerCount) / TotalCount;
	}
	else
	{
		SmokerRate = 0.0f;
		NonSmokerRate = 0.0f;
	}
}

void AResultManager::ApplyPaintResults()
{
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APC* PC = Cast<APC>(It->Get()))
		{
			PC->Client_UpdateResultUI(SmokerRate, NonSmokerRate);
		}
	}
	StartLerp();
	DidSmokerWin();
}

bool AResultManager::DidSmokerWin()
{
	bool bSmokerWin = SmokerRate > NonSmokerRate;

	if (AGS* GS = GetWorld()->GetGameState<AGS>())
	{
		GS->bSmokerWin = bSmokerWin;
	}

	return bSmokerWin;
}


