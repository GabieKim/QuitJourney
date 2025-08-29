// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "DataWrappers/ChaosVDParticleDataWrapper.h"
#include "GameFramework/Actor.h"
#include "PickUpActor.generated.h"

UCLASS()
class MULTITEST2_API APickUpActor : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> SphereCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Mesh;
	
public:	
	APickUpActor();

protected:
	virtual void BeginPlay() override;	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	
public:	
	virtual void Tick(float DeltaTime) override;

public:
	FORCEINLINE USphereComponent* GetSphereCollision()const {return SphereCollision;}
	FORCEINLINE UStaticMeshComponent* GetMesh()const {return Mesh;}
	
protected:
	// BlueprintNativeEvent : redefine ok @Blueprint
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pickup",
		meta = (DisplayName = "Pickup"))
	void Pickup(class AMyCharacter* OwningCharacter);
};
