// Fill out your copyright notice in the Description page of Project Settings.


#include "../Item/BaseWeaponProjectile.h"
#include "../MyCharacter.h"
#include "../CharacterStat.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABaseWeaponProjectile::ABaseWeaponProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(FName("Collision"));
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetSphereRadius(10.0f);
	SphereCollision->BodyInstance.SetCollisionProfileName("BlockAll");
	SphereCollision->OnComponentHit.AddDynamic(this, &ABaseWeaponProjectile::OnHit);
	
	RootComponent = SphereCollision;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Mesh->SetRelativeLocation(FVector(-40.f, 0.f, 0.f));
	Mesh->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	// 메시는 BP에서 넣는다

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = SphereCollision;
	ProjectileMovement->ProjectileGravityScale = 0; // 0이면 중력 무시
	ProjectileMovement->InitialSpeed = 3000.f; // 3000 cm/sec = 초속 30미터
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true; // 발사체가 이동방향으로 회전
	ProjectileMovement->bShouldBounce = false; // 안 튕김

	bReplicates = true;	
}

void ABaseWeaponProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseWeaponProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseWeaponProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	auto ComputedDamage = Damage;
	if (const auto Character = Cast<AMyCharacter>(GetInstigator()))
	{
		ComputedDamage*= Character->GetCharacterStat()->DamageMultiplier;
	}
	if (OtherActor != nullptr && OtherActor != this)
	{
		const FDamageEvent Event(UDamageType::StaticClass());
		OtherActor->TakeDamage(ComputedDamage, Event, GetInstigatorController(), this);
	}
	Destroy();
}


