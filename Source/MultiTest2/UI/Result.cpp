// Fill out your copyright notice in the Description page of Project Settings.


#include "../UI/Result.h"

#include "EndingText.h"
#include "EngineUtils.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "MultiTest2/GS.h"
#include "MultiTest2/PC.h"
#include "MultiTest2/GameSystem/ResultManager.h"

class AResultManager;

void UResult::NativeConstruct()
{
	Super::NativeConstruct();

	for (TActorIterator<AResultManager> It(GetWorld()); It; ++It)
	{
		ResultManager = *It;
		break;
	}	
}

void UResult::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Smoker 보간
	float CurSmoker = PB_Smoker->GetPercent();
	float NewSmoker = FMath::FInterpTo(CurSmoker, TargetSmokerRate, InDeltaTime, 1.f);
	PB_Smoker->SetPercent(NewSmoker);

	float DisplaySmoker = FMath::FInterpTo(
		FCString::Atof(*Txt_SmokerValue->GetText().ToString()),
		TargetSmokerRate * 100.f,
		InDeltaTime,
		1.f
	);
	Txt_SmokerValue->SetText(FText::AsNumber(FMath::RoundToDouble(DisplaySmoker * 10) / 10.f));

	// NonSmoker 보간
	float CurNonSmoker = PB_NonSmoker->GetPercent();
	float NewNonSmoker = FMath::FInterpTo(CurNonSmoker, TargetNonSmokerRate, InDeltaTime, 1.f);
	PB_NonSmoker->SetPercent(NewNonSmoker);

	float DisplayNonSmoker = FMath::FInterpTo(
		FCString::Atof(*Txt_NonSmokerValue->GetText().ToString()),
		TargetNonSmokerRate * 100.f,
		InDeltaTime,
		1.f
	);
	Txt_NonSmokerValue->SetText(FText::AsNumber(FMath::RoundToDouble(DisplayNonSmoker * 10) / 10.f));
	
	const bool bSmokerDone = FMath::IsNearlyEqual(PB_Smoker->GetPercent(), TargetSmokerRate, 0.01f);
	const bool bNonSmokerDone = FMath::IsNearlyEqual(PB_NonSmoker->GetPercent(), TargetNonSmokerRate, 0.01f);

	if (!bResultHandled && bSmokerDone && bNonSmokerDone)
	{
		bResultHandled = true;
		if (AGS* GS = GetWorld()->GetGameState<AGS>())
		{
			const bool bSmokerWin = GS->bSmokerWin;
			bSmokerWin ? EndingTextWidget->ShowQuitFailText() : EndingTextWidget->ShowQuitSucText();
			PlayEndingTextSound(bSmokerWin);
		}
		FTimerHandle DestroyHandle;
		GetWorld()->GetTimerManager().SetTimer(DestroyHandle, [this]()
		{
			this->RemoveFromParent();
			//UE_LOG(LogTemp, Warning, TEXT("ResultWidget destroyed after text show"));
			if (APlayerController* PC = GetOwningPlayer())
			{
				if (APC* MyPC = Cast<APC>(PC))
				{
					MyPC->Server_NotifyResultWidgetClosed(); // ← 여기에 서버로 신호!
				}
			}
		}, 3.0f, false); // ← 3초 뒤 실행
	}
}

void UResult::PlayEndingTextSound(bool bSmokerWin)
{
	if (bSmokerWin && SmokerWinSound)
	{
		UGameplayStatics::PlaySound2D(this, SmokerWinSound);
	}
	else if (!bSmokerWin && SmokerLoseSound)
	{
		UGameplayStatics::PlaySound2D(this, SmokerLoseSound);
	}
}
