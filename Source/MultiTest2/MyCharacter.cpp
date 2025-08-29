// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PC.h"
#include "PS.h"
#include "CharacterStat.h"
#include "Engine/DataTable.h"
#include "Item/Interactable.h"
#include "Item/WaeponProjectileComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GI.h"
#include "CharacterSkins.h"
#include "Item/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Slate/SGameLayerManager.h"
#include "Paint/PaintableActor.h"
#include "UI/Ult.h"
#include "UI/UltAlert.h"
#include "UI/PlayerIcon.h"
#include "EngineUtils.h"
//#include "SNegativeActionButton.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/WidgetComponent.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"
#include "GameSystem/PaintManager.h"
#include "PaperSpriteComponent.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 800.0f;
	CameraBoom->bUsePawnControlRotation = true;
	bUseControllerRotationYaw = false; // 캐릭터는 회전 X

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));
	NoiseEmitter->NoiseLifetime = 0.01f;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->InitCapsuleSize(100.f, 96.0f);

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	Weapon = CreateDefaultSubobject<UWaeponProjectileComponent>(TEXT("Weapon"));
	Weapon -> SetupAttachment(RootComponent);
	Weapon -> SetRelativeLocation(FVector(120.0f, 70.0f, 0.0f));
	Weapon -> SetIsReplicated(true);

	PlayerType = EPlayerType::None; 
	PlayerTypeText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PlayerTypeText"));
	PlayerTypeText->SetupAttachment(RootComponent); // 캐릭터에 붙이기
	PlayerTypeText->SetHorizontalAlignment(EHTA_Center);
	PlayerTypeText->SetWorldSize(50.0f); // 텍스트 크기 조정
	PlayerTypeText->SetTextRenderColor(FColor::White);
	PlayerTypeText->SetVisibility(true);
	PlayerTypeText->SetRelativeLocation(FVector(0.f, 0.f, 100.f));

	RoleIconWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("RoleIconWidget"));
	RoleIconWidget->SetupAttachment(GetMesh()); // 또는 RootComponent도 가능	
	RoleIconWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RoleIconWidget->SetWidgetSpace(EWidgetSpace::Screen);
	RoleIconWidget->SetDrawSize(FVector2D(30.f, 30.f));
	RoleIconWidget->SetDrawAtDesiredSize(true);
	RoleIconWidget->SetPivot(FVector2D(0.5f, 0.5f));
	RoleIconWidget->SetRelativeLocation(FVector(0.f, 0.f, 231.f));
	RoleIconWidget->SetVisibility(true);
	RoleIconWidget->SetIsReplicated(true);

	PaintFx= CreateDefaultSubobject<UNiagaraComponent>(TEXT("PaintNiagara"));
	PaintFx->SetupAttachment(RootComponent);
	PaintFx->Deactivate();
	StunFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("StunNiagara"));
	StunFX->SetupAttachment(RootComponent);
	StunFX->Deactivate();

	DustFX= CreateDefaultSubobject<UNiagaraComponent>(TEXT("Dust"));
	DustFX->SetupAttachment(RootComponent);
	DustFX->Deactivate();
	
	// 씬캡처 컴포넌트 생성
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(GetMesh());
	
	MinimapIndicator = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("PlayerSprite"));
	MinimapIndicator->SetupAttachment(GetMesh());
	MinimapIndicator->SetSprite(DefaultSprite);

	MinimapIndicator->SetIsReplicated(true);
}



void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext,0);
		}
	}
	
	UpdateCharacterStat(1);	
	
	if (RoleIconWidget && RoleIconWidgetClass)
	{
		UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(GetWorld(), RoleIconWidgetClass);
		RoleIconWidget->SetWidget(CreatedWidget);
	}
	
	if (IsLocallyControlled())
	{
		const auto GameInstanceCast = Cast<UGI>(GetWorld()->GetGameInstance());
		
		if (GameInstanceCast != nullptr)
		{
			SetSkinIdx_Server(GameInstanceCast->SkinIdx);
		}
		
		CreateUltWidget();	
	}
	
	PaintFx->Deactivate();
	StunFX->Deactivate();
	
	if (HasAuthority()) // 서버 본인일 때
	{
		if (APS* MyPS = GetPlayerState<APS>())
		{
			PlayerType = MyPS->PlayerType; // 명시적으로 지정
			OnRep_PlayerType();            // 위젯도 즉시 반영
		}
	}
	
	if (FootRenderTarget && SceneCapture)
	{
		SceneCapture->TextureTarget = FootRenderTarget;
	}
	
	MinimapIndicator->bOwnerNoSee = false;
	MinimapIndicator->bOnlyOwnerSee = false;
	MinimapIndicator->SetVisibility(true);
	MinimapIndicator->SetHiddenInGame(false);
}

/*
 * 기능: 복제되야할 변수 네크워크에서 복제 되도록 설정해주는 함수
 */
void AMyCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMyCharacter, SkinIdx);
	DOREPLIFETIME(AMyCharacter, bIsSmoker);
	DOREPLIFETIME(AMyCharacter, bCanUseUlt);
	DOREPLIFETIME(AMyCharacter, bIsStunned);
	DOREPLIFETIME(AMyCharacter, HitCount);
	DOREPLIFETIME(AMyCharacter, PlayerType);
	DOREPLIFETIME(AMyCharacter, bCanPaint);
}

void AMyCharacter::OnRep_PlayerType()
{
	UpdateHeadIcon(PlayerType);
	UpdateMinimapIcon(PlayerType);
	SetPlayerFXColor(PlayerType);	
}

void AMyCharacter::SetPlayerFXColor(EPlayerType NewType)
{
	switch (NewType)
	{
	case EPlayerType::Smoker:
		if (PaintFx && PaintFx_Smoker && StunFX)
		{
			PaintFx->SetAsset(PaintFx_Smoker);
			StunFX->SetVariableLinearColor(FName("User.MyColor"), NonSmokerColor);
		}
		break;

	case EPlayerType::NonSmoker:
		if (PaintFx && PaintFx_NonSmoker && StunFX)
		{
			PaintFx->SetAsset(PaintFx_NonSmoker);
			StunFX->SetVariableLinearColor(FName("User.MyColor"), SmokerColor);
		}
		break;
		
	default:
		break;
	}
}

void AMyCharacter::SetPlayerType_Implementation(EPlayerType NewType)
{
	if (!HasAuthority()) return; // 서버에서만 실행
	if (PlayerType == NewType) return; // 이미 같은 타입이면 무시

	PlayerType = NewType;
	
	if (IsLocallyControlled()) // 클라에서만 GameInstance 저장
	{
		if (UGI* GI = Cast<UGI>(GetGameInstance()))
		{
			if (!GI->SkinIdxMap.Contains(PlayerType))
			{
				GI->SkinIdxMap.Add(PlayerType, SkinIdx);
			}
		}
	}

	// 외형은 OnRep으로 처리됨
	Multicast_UpdatePlayerVisuals();
}

void AMyCharacter::Multicast_UpdatePlayerVisuals_Implementation()
{
	UpdateHeadIcon(PlayerType);			// 머리 위 아이콘
	UpdateMinimapIcon(PlayerType);		// 미니맵 인디케이터 스프라이트임
	SetPlayerFXColor(PlayerType);
}

void AMyCharacter::HidePlayerVisuals()
{
	if (RoleIconWidget)
	{
		RoleIconWidget->SetVisibility(false);
	}
	
	if (MinimapIndicator)
	{
		MinimapIndicator->SetVisibility(false); 
	}
	
	// 메시 숨기기 (캐릭터 본체)
	if (GetMesh())
	{
		GetMesh()->SetVisibility(false);
		GetMesh()->SetHiddenInGame(true); // 렌더링 완전 끄기
	}

	if (Weapon)
	{
		Weapon->SetVisibility(false);
		Weapon->SetHiddenInGame(true);
	}
}

void AMyCharacter::UpdateHeadIcon(EPlayerType NewType)
{
	if (RoleIconWidget)
	{
		if (UPlayerIcon* Icon = Cast<UPlayerIcon>(RoleIconWidget->GetUserWidgetObject()))
		{
			Icon->SetRoleIcon(NewType);
		}		
	}	
}

void AMyCharacter::UpdateMinimapIcon(EPlayerType NewType)
{
	if (!MinimapIndicator) return;
	
	switch (NewType)
	{
	case EPlayerType::Smoker:
		MinimapIndicator->SetSprite(SmokerSprite);
		//UE_LOG(LogTemp, Warning, TEXT("[Sprite] %s: Set to SmokerSprite"), *GetName());
		break;

	case EPlayerType::NonSmoker:
		MinimapIndicator->SetSprite(NonSmokerSprite);
		//UE_LOG(LogTemp, Warning, TEXT("[Sprite] %s: Set to NonSmokerSprite"), *GetName());
		break;

	case EPlayerType::None:
		MinimapIndicator->SetSprite(DefaultSprite);
		//UE_LOG(LogTemp, Warning, TEXT("[Sprite] %s: Set to DefaultSprite (None)"), *GetName());
		break;
	}
}


void AMyCharacter::SetCanPaint_Implementation(bool bEnable)
{
	bCanPaint = bEnable;
}

void AMyCharacter::Move(const struct FInputActionValue& Value)
{
	SceneCapture->CaptureScene(); // 캡처 먼저!
	const auto MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const auto Rotation = Controller->GetControlRotation();
		const FRotator YawRotation = FRotator(0.0f, Rotation.Yaw, 0.0f);
		const auto ForwardDirection = FRotationMatrix(YawRotation). GetUnitAxis(EAxis::X);
		const auto RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
	
}

void AMyCharacter::Look(const struct FInputActionValue& Value)
{
	const auto LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMyCharacter::SprintStart(const struct FInputActionValue& Value)
{
	// GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, TEXT("SprintStart"));
	if (GetCharacterStat())
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStat()->SprintSpeed;

		float SprintSpeed = GetCharacterStat()->SprintSpeed;
		//GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue,
				//FString::Printf(TEXT("SprintSpeed: %.f"), SprintSpeed));  
	}

	SprintStart_Server();
}

void AMyCharacter::SprintStart_Server_Implementation()
{
	if (GetCharacterStat())
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStat()->SprintSpeed;
	}
	SprintStart_Client();
}

void AMyCharacter::SprintStart_Client_Implementation()
{
	if (GetCharacterStat())
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStat()->SprintSpeed;
	}
}

void AMyCharacter::SprintEnd(const struct FInputActionValue& Value)
{
	// GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, TEXT("SprintEnd"));
	if (GetCharacterStat())
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStat()->WalkSpeed;

		float WalkSpeed = GetCharacterStat()->WalkSpeed;;
		//GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue,
			//	FString::Printf(TEXT("WalkSpeed: %.f"), WalkSpeed));  
	}
	
	SprintEnd_Server();
}

void AMyCharacter::SprintEnd_Server_Implementation()
{
	if (GetCharacterStat())
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStat()->WalkSpeed;
	}
	SprintEnd_Client();
}

void AMyCharacter::SprintEnd_Client_Implementation()
{
	if (GetCharacterStat())
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStat()->WalkSpeed;
	}
}

void AMyCharacter::Interact(const struct FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Red, TEXT("Interact"));
	Interact_Server();
}

void AMyCharacter::Interact_Server_Implementation()
{
	if (InteractableActor)
	{
		IInteractable::Execute_Interact(InteractableActor, this);
	}
}

void AMyCharacter::MyStopJumping(const struct FInputActionValue& Value)
{
	StopJumping();
}

void AMyCharacter::MyJump(const struct FInputActionValue& Value)
{
	SceneCapture->CaptureScene(); // 캡처 먼저!
	Jump();
}

void AMyCharacter::ApplyUltimateStun(AMyCharacter* Target)
{
	if (Target)
		ApplyUltimateStun_Server(Target);
}

void AMyCharacter::ApplyUltimateStun_Server_Implementation(AMyCharacter* Target)
{
	if (Target)
		ApplyUltimateStun_Multicast(Target);
}

void AMyCharacter::ApplyUltimateStun_Multicast_Implementation(AMyCharacter* Target)
{
	if (!Target) return;
	
	Target->bIsStunned = true;
	Target->GetCharacterMovement()->DisableMovement();
	Target->PlayReactionMontage_Multicast(4);
	UE_LOG(LogTemp, Display, TEXT("stun start"));

	if (Target->IsLocallyControlled())
	{
		if (Target->UltAlertWidgetClass)
		{
			Target->UltAlertWidget = CreateWidget<UUltAlert>(Target->GetWorld(), Target->UltAlertWidgetClass);
			if (Target->UltAlertWidget)
			{
				Target->UltAlertWidget->AddToViewport();
			}
		}
	}
	if (CoughSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CoughSound, GetActorLocation());
	}
	if (StunFX)
	{
		Target->StunFX->Activate();
	}
	
	FTimerHandle Handle;
	Target->GetWorld()->GetTimerManager().SetTimer(Handle, [Target]()
	{
		Target->bIsStunned = false;
		Target->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		UE_LOG(LogTemp, Display, TEXT("stun finish"));
		
		// 위젯 제거 추가
		if (Target->UltAlertWidget && Target->UltAlertWidget->IsInViewport())
		{
			Target->UltAlertWidget->RemoveFromParent();
			Target->UltAlertWidget = nullptr; // 메모리 정리 차원
		}
	}, 3.f, false);
}

/*
 * 기능 : 멤버변수 SkinIdx 값이 변경될 때마다 자동으로 호출됨 
 */
void AMyCharacter::OnRep_SkinChanged()
{
	//UE_LOG(LogTemp, Warning, TEXT("OnRep_SkinChanged() 실행됨! SkinIdx: %d"), SkinIdx);
	UpdateCharacterSkin();
}

/*
* 기능 : 멤버변수 SkinIdx에 따라 캐릭터의 머티리얼을 변경
 */
void AMyCharacter::UpdateCharacterSkin()
{
	if (!CharacterSkinDataTable || SkinIdx < 0) return;  // 데이터 테이블이 없거나 유효하지 않은 인덱스면 종료

	// 현재 선택된 스킨을 가져옴
	CharacterSkins = GetSkinByIndex(SkinIdx);
	if (!CharacterSkins) return; 

	// 머티리얼 리스트 가져오기
	TArray<UMaterialInterface*> Materials = CharacterSkins->GetMaterialArray();

	// 머티리얼이 있을 때만 적용
	for (int32 i = 0; i < Materials.Num(); i++)
	{
		if (Materials[i])
		{
			GetMesh()->SetMaterial(i, Materials[i]);
		}
	}

}

void AMyCharacter::UpdateCharacterSkinForSeq(int32 Idx)
{
	if (!CharacterSkinDataTable || Idx < 0) return;  // 데이터 테이블이 없거나 유효하지 않은 인덱스면 종료

	// 현재 선택된 스킨을 가져옴
	CharacterSkins = GetSkinByIndex(Idx);
	if (!CharacterSkins) return; 

	// 머티리얼 리스트 가져오기
	TArray<UMaterialInterface*> Materials = CharacterSkins->GetMaterialArray();

	// 머티리얼이 있을 때만 적용
	for (int32 i = 0; i < Materials.Num(); i++)
	{
		if (Materials[i])
		{
			GetMesh()->SetMaterial(i, Materials[i]);
		}
	}
}

void AMyCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (APS* MyPS = GetPlayerState<APS>())
	{
		PlayerType = MyPS->PlayerType;
		OnRep_PlayerType(); // 안전하게 UI, 이펙트 갱신
	}
}



void AMyCharacter::CreateUltWidget()
{
	if (UltWidgetClass)
	{
		UltWidget = CreateWidget<UUlt>(GetWorld(), UltWidgetClass);
		UltWidget->AddToViewport();
		if (UltWidget)
		{
			UltWidget->UpdateUltimateCharge(0.0f); // 여기서도 0으로 초기화
		}
	}
}

void AMyCharacter::RemoveUltWidget()
{
	if (UltWidget)
	{
		UltWidget->RemoveFromParent();
		//UE_LOG(LogTemp, Warning, TEXT("UltWidget real remove from parent"));
		UltWidget = nullptr;
		//UE_LOG(LogTemp, Warning, TEXT("UltWidget is null"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UltWidget is already null"));
	}
}

void AMyCharacter::HideWeapon_Implementation(EPlayerType NewType)
{
	if (!HasAuthority()) return; // 서버에서만 실행
	PlayerType = NewType;

	if (CurrentWeapon)
	{
		MulticastHideWeapon(CurrentWeapon, 
			(PlayerType == EPlayerType::Smoker) ? TEXT("CigaretteSocket") : TEXT("PistolSocket"));
	}
}

void AMyCharacter::MulticastHideWeapon_Implementation(AWeapon* SpawnedWeapon, FName SocketName)
{
	if (SpawnedWeapon)
	{
		// 가장 확실하게 숨기는 방식
		SpawnedWeapon->SetActorHiddenInGame(true);
		SpawnedWeapon->SetActorEnableCollision(false);
		SpawnedWeapon->SetActorTickEnabled(false);
	}
}

void AMyCharacter::MulticastAttachWeapon_Implementation(AWeapon* SpawnedWeapon, FName SocketName)
{
	if (!SpawnedWeapon) return;

	//UE_LOG(LogTemp, Warning, TEXT("클라이언트에서 MulticastAttachWeapon 실행됨!"));

	// 클라이언트에서도 무기 부착
	SpawnedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
}

void AMyCharacter::SpawnAndAttachWeapon_Implementation(EPlayerType NewType)
{
	PlayerType = NewType; 		
	
	if (!HasAuthority()) return; // 서버에서만 실행
	
	// 플레이어 타입이 None이면 무기 스폰 X
	if (PlayerType == EPlayerType::None)
	{
		return;
	}

	// 플레이어 타입에 따른 무기 및 소켓 설정
	TSubclassOf<AWeapon> WeaponToSpawn = (PlayerType == EPlayerType::Smoker) ? SmokerWeapon : NonSmokerWeapon;
	FName SocketName = (PlayerType == EPlayerType::Smoker) ? TEXT("CigaretteSocket") : TEXT("PistolSocket");

	if (WeaponToSpawn)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
		// 스폰된 무기 복제 활성화
		AWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponToSpawn, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
		CurrentWeapon = SpawnedWeapon; // 나중에 안보일 때 알아햐하니까 저장해줌
		
		if (SpawnedWeapon)
		{
			// 무기 복제 가능하도록 설정 (필수)
			SpawnedWeapon->SetReplicates(true);
			SpawnedWeapon->SetReplicateMovement(true);

			// 서버에서 무기 부착
			SpawnedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);

			// 멀티캐스트 호출해서 클라이언트에서도 무기 부착
			MulticastAttachWeapon(SpawnedWeapon, SocketName);
		}
	}
}


void AMyCharacter::SetSkinIdx_Server_Implementation(int32 Value)
{
	// UE_LOG(LogTemp, Warning, TEXT("[서버] SetSkinIdx_Server 실행됨, Value: %d"), Value);	
	SkinIdx = Value;
	UpdateCharacterSkin();
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 권한 확인 먼저!
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EMC =
		Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EMC->BindAction(MoveAction,ETriggerEvent::Triggered,this, &AMyCharacter::Move);
		EMC->BindAction(LookAction,ETriggerEvent::Triggered,this, &AMyCharacter::Look);
		EMC->BindAction(InteractAction,ETriggerEvent::Started,this, &AMyCharacter::Interact);
		EMC->BindAction(SprintAction,ETriggerEvent::Started,this, &AMyCharacter::SprintStart);
		EMC->BindAction(SprintAction,ETriggerEvent::Completed,this, &AMyCharacter::SprintEnd);
		EMC->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMyCharacter::Jump);
		EMC->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::StopJumping);
		EMC->BindAction(AttackAction, ETriggerEvent::Started, this, &AMyCharacter::PerformAttack);
		EMC->BindAction(UltAction, ETriggerEvent::Started, this, &AMyCharacter::UseUltimate);
	}
}



void AMyCharacter::PlayReactionMontage(int32 MontageIndex)
{
	if (AttackMontages.IsValidIndex(MontageIndex))
	{
		PlayAnimMontage(AttackMontages[MontageIndex]);
	}
}

void AMyCharacter::PlayReactionMontage_Multicast_Implementation(int32 MontageIndex)
{
	PlayReactionMontage(MontageIndex);
}

void AMyCharacter::OnRep_CanUseUlt()
{
	//UE_LOG(LogTemp, Warning, TEXT("클라에서 bCanUseUlt 동기화됨: %s"), bCanUseUlt ? TEXT("true") : TEXT("false"));
	UpdateUIClient(HitCount);
}

void AMyCharacter::OnRep_HitCount()
{
	UpdateUIClient(HitCount);
	//UE_LOG(LogTemp, Warning, TEXT("OnRep_HitCount Call"));
}

bool AMyCharacter::IsOnPaintedSurface() const
{
	ESurfaceType SurfaceType = GetSurfaceTypeUnderFoot();
	return SurfaceType != ESurfaceType::Default;
}

void AMyCharacter::PlayFootstep(EFootstepType FootstepType)
{
	// 클라이언트에서 본인만 사운드 재생
	if (!IsLocallyControlled()) return;
	SceneCapture->CaptureScene();
	
	ESurfaceType Surface = GetSurfaceTypeUnderFoot(); // ← 반드시 내 렌더타겟을 기준으로만 실행됨

	const FFootstepSurfaceSounds* SoundSet = FootstepSounds.Find(FootstepType);
	if (!SoundSet) return;

	const TArray<USoundBase*>& Sounds = (Surface != ESurfaceType::Default)
		? SoundSet->PaintedSounds
		: SoundSet->DefaultSounds;

	if (Sounds.Num() == 0) return;

	// 중복 방지용 랜덤 선택
	int32& LastIndex = LastPlayedSoundIndex.FindOrAdd(FootstepType);
	int32 NewIndex;
	do {
		NewIndex = FMath::RandRange(0, Sounds.Num() - 1);
	} while (NewIndex == LastIndex && Sounds.Num() > 1);

	LastIndex = NewIndex;

	// 본인 클라이언트에서만 사운드 재생
	UGameplayStatics::PlaySoundAtLocation(this, Sounds[NewIndex], GetActorLocation());
}


ESurfaceType AMyCharacter::GetSurfaceTypeUnderFoot() const
{
	if (!FootRenderTarget) return ESurfaceType::Default;

	FTextureRenderTargetResource* RTResource = FootRenderTarget->GameThread_GetRenderTargetResource();
	if (!RTResource) return ESurfaceType::Default;

	TArray<FColor> PixelColor;
	FReadSurfaceDataFlags ReadFlags(RCM_UNorm);
	ReadFlags.SetLinearToGamma(false);

	FIntRect ReadRect(128, 128, 129, 129);
	if (!RTResource->ReadPixels(PixelColor, ReadFlags, ReadRect) || PixelColor.Num() == 0)
		return ESurfaceType::Default;

	FLinearColor Color = FLinearColor(PixelColor[0]);
	const float TOL = 0.05f;

	// 디버깅용 로그
	//UE_LOG(LogTemp, Warning, TEXT("UnderFoot Color: R=%.3f G=%.3f B=%.3f"), Color.R, Color.G, Color.B);

	// NonSmoker 페인트
	if (FMath::IsNearlyEqual(Color.R, 0.0f, TOL)
		&& FMath::IsNearlyEqual(Color.G, 0.54f, TOL)
		&& FMath::IsNearlyEqual(Color.B, 1.0f, TOL))
	{
		return ESurfaceType::NonsmokerPainted;
	}

	// Smoker 페인트
	if (FMath::IsNearlyEqual(Color.R, 0.0f, TOL)
		&& FMath::IsNearlyEqual(Color.G, 0.0f, TOL)
		&& FMath::IsNearlyEqual(Color.B, 0.0f, TOL))
	{
		return ESurfaceType::SmokerPainted;
	}

	// 기본 바닥
	return ESurfaceType::Default;
}

void AMyCharacter::OnHitEnemy()
{
	if (!HasAuthority()) return; // 서버에서만 하라는 뜻!
	
	// 맞춘 횟수 증가
	HitCount++;
	UE_LOG(LogTemp, Warning, TEXT("%d"), HitCount);
		
	// 궁극기 UI 업데이트
	UpdateUIClient(HitCount);

	// 궁극기 충전 완료
	if (HitCount >= 5)
	{
		bCanUseUlt = true;
	}
}

void AMyCharacter::UseUltimate()
{
	//if (!HasAuthority()) return;
	
	UE_LOG(LogTemp, Warning, TEXT("UseUltimate Called"));
	if (!bCanUseUlt)
	{
		UE_LOG(LogTemp, Warning, TEXT("BeCanUseUlt = false"));
		return;		
	}	
	
	// 궁극기 효과 실행
	UE_LOG(LogTemp, Warning, TEXT("Ult Used"));
	// 내 말고 다른 플레이어 한 명만 찾기
	for (TActorIterator<AMyCharacter> It(GetWorld()); It; ++It)
	{
		AMyCharacter* Other = *It;
		if (Other != this && Other->PlayerType != this->PlayerType)
		{
			ApplyUltimateStun_Server(Other); // 바로 서버 함수 호출
			break; // 한 명만 있으니까 바로 끝내기
		}
	}
	
	// 초기화
	HitCount = 0;
	bCanUseUlt = false;
	UE_LOG(LogTemp, Warning, TEXT("After UseUltmate 초기화"));
	UpdateUIClient(HitCount);
}

void AMyCharacter::UpdateUIClient_Implementation(int32 CurrentHits)
{
	if (UltWidget)
	{
		UltWidget->UpdateUltimateCharge(CurrentHits);
		//UE_LOG(LogTemp, Warning, TEXT("UltWidget is not null"));
	}
	
}

void AMyCharacter::UpdateCharacterStat(int32 CharacterLevel)
{
	auto IsSprinting = false;

	if (GetCharacterStat())
	{
		IsSprinting = GetCharacterMovement()->MaxWalkSpeed == GetCharacterStat()->SprintSpeed;
	}
	
	if (CharacterDataTable)
	{
		TArray<FCharacterStat*> CharacterStatsRows;
		CharacterDataTable->GetAllRows<FCharacterStat>(TEXT("Character"), CharacterStatsRows);

		if (CharacterStatsRows.Num() > 0)
		{
			const auto NewCharacterLevel = FMath::Clamp(CharacterLevel, 1,
				CharacterStatsRows.Num());
			CharacterStat = CharacterStatsRows[CharacterLevel - 1];

			GetCharacterMovement()->MaxWalkSpeed = GetCharacterStat()->WalkSpeed;
			
			if (IsSprinting)
			{
				SprintStart_Server();
			}
		}
	}
}


FCharacterSkins* AMyCharacter::GetSkinByIndex(int32 Index) const
{
	if (!CharacterSkinDataTable) return nullptr;  // 데이터 테이블이 없으면 nullptr 반환

	TArray<FCharacterSkins*> CharacterSkinsRows;
	CharacterSkinDataTable->GetAllRows<FCharacterSkins>(TEXT("MyCharacter"), CharacterSkinsRows);

	if (CharacterSkinsRows.IsValidIndex(Index))  //  유효한 인덱스인지 확인
	{
		return CharacterSkinsRows[Index];
	}

	return nullptr;  //  잘못된 인덱스면 nullptr 반환
}

/*
 * 기능 : 좌클릭시 수행해야 하는 함수들 통합함
 *		  1)애님몽타주 실행
 *		  2)타격 판정
 */
void AMyCharacter::PerformAttack()
{
	PerformAttack_Server(); // 이건 서버에서 서버 함수를 직접 호출하는 것	
}

void AMyCharacter::PerformAttack_Server_Implementation()
{
	//int32 RandomIndex = FMath::RandRange(0, AttackMontages.Num() - 1);

	int32 Index = 0;
	PlayAttackMontage_Multicast(Index); // 모든 클라에서 재생
	Fire_Server(); // 서버에서 판정 후 멀티캐스트
}

void AMyCharacter::PlayAttackMontage_Multicast_Implementation(int32 MontageIndex)
{
	if (AttackMontages.IsValidIndex(MontageIndex))
	{
		PlayAnimMontage(AttackMontages[MontageIndex]);
	}
}

/*
 *  기능 1. 시작점, 끝점 계산 (카메라 기준)
	기능 2. 라인트레이스
	기능 3. 맞은 대상이 있으면
			→ 디버그 시각화
			→ 맞은 액터가 PaintableActor라면 → 페인트
			→ 맞은 액터가 MyCharacter라면 → 데미지 등
*/
void AMyCharacter::Fire_Server_Implementation()
{
	Multicast_PlayPaintEffect();
	Multicast_PlayFireSound();

	const FVector Start = FollowCamera->GetComponentLocation();
	const FVector End = Start + (FollowCamera->GetForwardVector() * 2000.0f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = true;
	Params.bReturnFaceIndex = true;

	const bool bHit = GetWorld()->LineTraceSingleByChannel
	(HitResult, Start, End, ECC_Visibility, Params);
	if (!bHit || !HitResult.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Fire: No hit detected."));
		return;
	}

	ProcessFireHit(HitResult);
}

void AMyCharacter::ProcessFireHit(const FHitResult& HitResult)
{
	AActor* HitActor = HitResult.GetActor();
	
	// 페인트 가능한 액터에 명중
	if (APaintableActor* PaintableActor = Cast<APaintableActor>(HitActor))
	{
		if (HitResult.FaceIndex == -1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Fire: Invalid FaceIndex."));
			return;
		}

		FVector2D UV;
		if (UGameplayStatics::FindCollisionUV(HitResult, 0, UV))
		{
			Fire_Multicast(PaintableActor, bIsSmoker, UV);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Fire: Failed to find collision UV."));
		}
	}
	// 다른 캐릭터 명중
	else if (AMyCharacter* DamagedCharacter = Cast<AMyCharacter>(HitActor))
	{
		OnHitEnemy();
		DamagedCharacter->PlayReactionMontage_Multicast(3);
	}
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Fire: Hit non-paintable, non-character actor."));
	// }
}

 void AMyCharacter::Fire_Multicast_Implementation(APaintableActor* Target, bool bIsSmoker2, FVector2D UV)
 {
	if (!Target || !bCanPaint) return;

	FLinearColor TeamColor = bIsSmoker2 ? SmokerColor : NonSmokerColor;
	Target->BePainted(bIsSmoker2, UV, TeamColor);
 }

void AMyCharacter::Multicast_PlayPaintEffect_Implementation()
{
	if (PaintFx&&bCanPaint)
	{
		PaintFx->Activate(true); // 이펙트 재생
	}
}

void AMyCharacter::Multicast_PlayFireSound_Implementation()
{
	if (CurrentWeapon && bCanPaint)
	{
		CurrentWeapon->PlayFireSound();
	}
}



