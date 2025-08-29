// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Minion.generated.h"

UCLASS()
class MULTITEST2_API AMinion : public ACharacter
{
	GENERATED_BODY()

public:
	AMinion();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MinionPerception", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPawnSensingComponent> PawnSense;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MinionPerception", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> Collision;
	UPROPERTY()
	FVector PatrolLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MinionAI", meta = (AllowPrivateAccess = "true"))
	float AlertRadius = 6000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float Health = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PickUp", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class APickUpActor> SpawnedPickup;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = "MinionAI")
	float PatrolSpeed = 150.f;
	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = "MinionAI")
	float ChaseSpeed = 350.f;
	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = "MinionAI")
	float PatrolRadius = 50000.f;

	UFUNCTION(BlueprintCallable, Category = "MinionAI")
	void SetNextPatrolLocation();
	UFUNCTION(BlueprintCallable, Category = "MinionAI")
	void Chase(APawn* Pawn);
	UFUNCTION()
	void OnPawnDetected(APawn* Pawn);
	UFUNCTION()
	void OnBeingOverlap(AActor* OvelappedActor, AActor* OtherActor);
	UFUNCTION(BlueprintCallable, Category = "MinionAI")
	void GotoLocation(const FVector& Location);
	
	virtual void PostInitializeComponents() override;
	
	FORCEINLINE UPawnSensingComponent* GetPawnSense()const{return PawnSense;};
	FORCEINLINE class USphereComponent* GetCollision()const{return Collision;};

protected:
	UFUNCTION()
	void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);
	UFUNCTION()
	void OnDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBody, AActor* DamageCauser);
};
