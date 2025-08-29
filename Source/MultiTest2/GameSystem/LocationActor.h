// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LocationActor.generated.h"

UCLASS()
class MULTITEST2_API ALocationActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALocationActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
