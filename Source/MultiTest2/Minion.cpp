// Fill out your copyright notice in the Description page of Project Settings.


#include "Minion.h"
#include "AIController.h"
#include "MyCharacter.h"
#include "GM.h"
#include "Item/PickUpActor.h"
#include "NavigationSystem.h"
//#include "ActorFactories/ActorFactorySkeletalMesh.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Chaos/PBDSuspensionConstraintData.h"
#include "Rendering/RenderCommandPipes.h"
#include "Slate/SGameLayerManager.h"

AMinion::AMinion()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass =AAIController::StaticClass();

	/*폰센스*/
	PawnSense = CreateDefaultSubobject<UPawnSensingComponent>(FName("PawnSense"));
	PawnSense->SensingInterval = 0.8f; // <- 감각 인식 할 때 첫번째와 두번째 시간의 차이임. 클수록 둔한 AI
	PawnSense->SetPeripheralVisionAngle(45.0f);
	PawnSense->SightRadius = 1500.0f;
	PawnSense->HearingThreshold = 400.0f;
	PawnSense->LOSHearingThreshold = 800.0f;

	Collision = CreateDefaultSubobject<USphereComponent>(FName("Collision"));
	Collision->SetSphereRadius(100.0f);
	Collision->SetupAttachment(RootComponent);

	GetCapsuleComponent()->InitCapsuleSize(60.0f, 96.0f);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -91.0f));
	// 미니언 메시는 BP에서 넣는다

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// 미니언 죽고 생기는 아이템 메시는 BP에서 넣는다
}

void AMinion::BeginPlay()
{
	Super::BeginPlay();

	SetNextPatrolLocation();	
}

void AMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (GetMovementComponent()->GetMaxSpeed() == ChaseSpeed) // 스피드로 정찰 중인지 확인
	{
		return;
	}
	if ((GetActorLocation() - PatrolLocation).Size() < 500.f) // 정찰 범위 0.5M 이내 인지
	{
		SetNextPatrolLocation();
	}

}

void AMinion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMinion::SetNextPatrolLocation()
{
	 if (GetLocalRole()!= ROLE_Authority)
	 {
		 return;
	 }

	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;

	const auto LocationFound =UNavigationSystemV1::
	K2_GetRandomReachablePointInRadius(
		this, GetActorLocation(), PatrolLocation, PatrolRadius);

	if (LocationFound)
	{
		// 목적지는 Location
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PatrolLocation);
	}
}

void AMinion::Chase(APawn* Pawn)
{
	if (GetLocalRole()!= ROLE_Authority)
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	// 목적지는 액터
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), Pawn);

	DrawDebugSphere(GetWorld(), Pawn->GetActorLocation(),
		25.f, 12, FColor::Red, true, 10.0f, 2.0f);

	if (const auto GameMode = Cast<AGM>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->AlertMinion(this, Pawn->GetActorLocation(), AlertRadius);
	}
}

void AMinion::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetLocalRole()!= ROLE_Authority)
	{
		return;
	}

	// 함수를 델리게이트에 바인딩 : 오버랩, 폰 인식했는지 확인 
	OnActorBeginOverlap.AddDynamic(this, &AMinion::AMinion::OnBeingOverlap);
	GetPawnSense()->OnSeePawn.AddDynamic(this, &AMinion::OnPawnDetected);	
	GetPawnSense()->OnHearNoise.AddDynamic(this, &AMinion::OnHearNoise);

	OnTakeAnyDamage.AddDynamic(this, &AMinion::OnDamage);
}

/*
 * 기능1 : 소음 감지했다고 디버그 메시지 출력
 * 기능2 : GoToLocation 함수 호출 (미니언을 로케이션으로 보냄) 
 */
void AMinion::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Noise Detected!");
	GotoLocation(Location);
}

/*
 * 기능1 : 건강을 공격 받은 만큼 뺀다
 * 기능2 : 스폰드픽엄템이 있으면(BP에서 설정해줘야 함) 스폰 시킴
 * 기능3 : (미니언) 죽는다
 */
void AMinion::OnDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBody,
	AActor* DamageCauser)
{
	Health -= Damage;
	if (Health > 0)
	{
		return;
	}
	if (SpawnedPickup)
	{
		GetWorld()->SpawnActor<APickUpActor>(SpawnedPickup, GetActorLocation(), GetActorRotation());
	}
	Destroy();	
}

/*
 * 기능1 : 폰 감지할 때 마다 MyCharacter인지 확인
 * 기능2 : 감지된 폰이 MyCharacter면 Chase 
 */
void AMinion::OnPawnDetected(APawn* Pawn)
{
	if (!Pawn->IsA<AMyCharacter>())
	{
		return;
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f,
	FColor::Red, TEXT("Character Detected!"));

	if (GetCharacterMovement()->MaxWalkSpeed != ChaseSpeed)
	{
		Chase(Pawn);
	}	
		
}

/*
 * 기능 : 캐릭터 감지하면(오버랩) 디버그 메시지 출력
 */
void AMinion::OnBeingOverlap(AActor* OvelappedActor, AActor* OtherActor)
{
	if (!OtherActor->IsA<AMyCharacter>())
	{
		return;
	}
		
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
			TEXT("Character Captured!"));			
}

/*
 * 기능 : 매개변수로 받은 Location을 PatrolLocation으로 정하고 순찰
 */
void AMinion::GotoLocation(const FVector& Location)
{
	PatrolLocation = Location;
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PatrolLocation);
}



