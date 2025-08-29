

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../MyCharacter.h"
#include "PaintManager.generated.h"


UCLASS()
class MULTITEST2_API APaintManager : public AActor
{
	GENERATED_BODY()
	
public:	
	APaintManager();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void SetColorsFromMyCharacter();
	
public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	ESurfaceType GetSurfaceTypeAtLocation(const FVector& WorldLocation) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint|TeamColor")
	FLinearColor SmokerColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint|TeamColor")
	FLinearColor NonsmokerColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Paint")
	UTextureRenderTarget2D* PaintRenderTarget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Paint")
	AActor* PaintOriginReference;

	// 전체 페인팅 가능한 영역의 월드 크기 (cm)
	UPROPERTY(EditAnywhere, Category = "Paint")
	FVector2D WorldPaintSize = FVector2D(1000.0f, 1000.0f);
	UPROPERTY(EditAnywhere, Category = "Paint")
	FLinearColor MyTeamColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Paint")
	USceneCaptureComponent2D* SceneCapture;

private:
	// 월드 좌표 → UV 좌표 변환
	FVector2D ConvertWorldToUV(const FVector& WorldPos) const;
};
