// Fill out your copyright notice in the Description page of Project Settings.


#include "../Item/PickUpActor.h"
#include "../MyCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"


APickUpActor::APickUpActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(FName("SphereCollision"));
	RootComponent = SphereCollision;
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetSphereRadius(200.f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	Mesh->SetupAttachment(SphereCollision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	bReplicates = true;
}

void APickUpActor::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &APickUpActor::OnBeginOverlap);
}

void APickUpActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult &SweepResult)
{
	if (const auto Character  = Cast<AMyCharacter>(OtherActor))
	{
		Pickup(Character);
	}
}

void APickUpActor::Pickup_Implementation(class AMyCharacter* OwningCharacter)
{
	SetOwner(OwningCharacter);
}

void APickUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

