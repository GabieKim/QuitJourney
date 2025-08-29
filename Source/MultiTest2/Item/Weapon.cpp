// Fill out your copyright notice in the Description page of Project Settings.


#include "../Item/Weapon.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true; // 액터 복제
	SetReplicateMovement(true);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("WeaponMesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetIsReplicated(true); // 메시 복제
	
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::PlayFireSound()
{
	Multicast_PlayFireSound();
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeapon, Mesh);
}

void AWeapon::OnRep_WeaponMesh()
{
	//UE_LOG(LogTemp, Warning, TEXT("WeaponClass : OnRep_WeaponMesh Call"));
	
	if (Mesh)
	{
		Mesh->SetVisibility(true);  // 클라이언트에서 메시가 보이도록 설정
	}
}

void AWeapon::Multicast_PlayFireSound_Implementation()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		//UE_LOG(LogTemp, Warning, TEXT("FireSound Call"));
	}
}
