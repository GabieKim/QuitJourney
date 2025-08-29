// Fill out your copyright notice in the Description page of Project Settings.


#include "../Item/WaeponProjectileComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include  "BaseWeaponProjectile.h"
#include "../MyCharacter.h"


UWaeponProjectileComponent::UWaeponProjectileComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ProjectileClass = ABaseWeaponProjectile::StaticClass();
	
}

void UWaeponProjectileComponent::BeginPlay()
{
	Super::BeginPlay();

	const ACharacter* Character = Cast<ACharacter>(GetOwner());

	// 캐릭터 전용 컴포넌트라서 캐릭터 아니면 실행 X
	if (!Character)
	{
		return;
	}
	// 플레이어가 조종하는 캐릭터가 맞다면
	if (const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// 프라이어리이티 1? 기존 입력보다 우선순위 높음
			Subsystem->AddMappingContext(WeaponMappingContext,1);
		}		
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &UWaeponProjectileComponent::Throw);
		}
	}
}


void UWaeponProjectileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWaeponProjectileComponent::Throw()
{
	Throw_Server();
}

void UWaeponProjectileComponent::Throw_Server_Implementation()
{
	if (ProjectileClass)
	{
		Throw_Client();
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			const auto Character = Cast<AMyCharacter>(GetOwner());
			const auto ProjectileSpawnLocation = GetComponentLocation();
			const auto ProjectileSpawnRotation = GetComponentRotation();
			auto ProjectileSpawnParams = FActorSpawnParameters();
			ProjectileSpawnParams.Owner = GetOwner();
			ProjectileSpawnParams.Instigator = Character;
		
			GetWorld()->SpawnActor<ABaseWeaponProjectile>(ProjectileClass,
			ProjectileSpawnLocation,
			ProjectileSpawnRotation, 
			ProjectileSpawnParams);			
		}, .4f, false); // Throw 애님 보여주고 스폰시켜야하니까 
	}	
}

void UWaeponProjectileComponent::Throw_Client_Implementation()
{
	const auto Character = Cast<AMyCharacter>(GetOwner());
	if (ThrowAnim)
	{
		const auto AnimInstance = Character->GetMesh()->GetAnimInstance();
		
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(ThrowAnim, 1.f);
		}		
	}
}

void UWaeponProjectileComponent::SetProjectileClass(TSubclassOf<class ABaseWeaponProjectile> NewProjectileClass)
{
	ProjectileClass = NewProjectileClass;
}

