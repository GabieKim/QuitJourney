// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterSkins.h"
#include "GameFramework/Character.h"
#include "UI/UltAlert.h"
class APaintableActor;
class AWeapon;
class UUlt;
class UNiagaraSystem;
class APaintManager;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
#include "Animation/FootStep.h" // enum 공유
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "MyCharacter.generated.h"

UENUM(BlueprintType)  // 블루프린트에서도 사용 가능하도록 설정
enum class EPlayerType : uint8
{
	None	   UMETA(DisplayName = "??"),
	Smoker     UMETA(DisplayName = "Smoker"),   // 흡연자
	NonSmoker  UMETA(DisplayName = "NonSmoker") // 비흡연자
};


// 바닥 타입
UENUM(BlueprintType)
enum class ESurfaceType : uint8
{
	Default,
	SmokerPainted,
	NonsmokerPainted,
};

// 구조체: 바닥 종류에 따라 다른 사운드 배열
USTRUCT(BlueprintType)
struct FFootstepSurfaceSounds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<USoundBase*> DefaultSounds;

	UPROPERTY(EditAnywhere)
	TArray<USoundBase*> PaintedSounds;
};
UCLASS()
class MULTITEST2_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;
	
public:
	AMyCharacter();
	
protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	int32 GetSkinIdx() const {return SkinIdx;};
	
private:
	
	/* Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> UltAction;
	
	/* Data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData, meta = (AllowPrivateAccess = "true"))
	class UDataTable* CharacterDataTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacteterData", meta = (AllowPrivateAccess = "true"))
	class UDataTable* CharacterSkinDataTable;
	struct FCharacterStat* CharacterStat;
	struct FCharacterSkins* CharacterSkins;

	/* ETC */
	UPROPERTY()
	AActor* InteractableActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stealth", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPawnNoiseEmitterComponent> NoiseEmitter;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Waepon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWaeponProjectileComponent> Weapon;
	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* BrushMaterial;
	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UTextureRenderTarget2D* PaintRenderTarget;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character", meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* PlayerTypeText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> RoleIconWidget;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> RoleIconWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AWeapon> SmokerWeapon;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AWeapon> NonSmokerWeapon;

	/* Sprite for Minimap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite")
	class UPaperSpriteComponent* MinimapIndicator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite")
	class UPaperSprite* DefaultSprite;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite")
	class UPaperSprite* SmokerSprite;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite")
	class UPaperSprite* NonSmokerSprite;
	
	/* SceneCaptue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	USceneCaptureComponent2D* SceneCapture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	UTextureRenderTarget2D* FootRenderTarget;
	
	/* Anim */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TArray<TObjectPtr<UAnimMontage>> AttackMontages;
	UFUNCTION()
	void PlayReactionMontage(int32 MontageIndex);
	UFUNCTION(NetMulticast, Reliable)
	void PlayReactionMontage_Multicast(int32 MontageIndex);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skin, ReplicatedUsing = "OnRep_SkinChanged")
	int32 SkinIdx = -1;

	/* Ult */
	UPROPERTY(ReplicatedUsing = OnRep_HitCount)
	int32 HitCount = 0;	
	UPROPERTY(ReplicatedUsing = OnRep_CanUseUlt)
	bool bCanUseUlt = false;
	UPROPERTY(Replicated)
	bool bIsStunned = false;
	UFUNCTION()
	void OnRep_CanUseUlt();
	UFUNCTION()
	void OnRep_HitCount();

	/* Widget */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUlt> UltWidgetClass;// 위젯 생성용 클래스	
	UPROPERTY()
	class UUlt* UltWidget;// 생성된 위젯 인스턴스

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUltAlert> UltAlertWidgetClass;
	UPROPERTY()
	class UUltAlert* UltAlertWidget;
	
	/* AnimNotify */
	/** 사운드 데이터: [Walk / Run / Land] × [Default / Painted] */
	UPROPERTY(EditAnywhere, Category = "Footstep")
	TMap<EFootstepType, FFootstepSurfaceSounds> FootstepSounds;
	/* 마지막 재생 인덱스 (중복 방지용) */
	TMap<EFootstepType, int32> LastPlayedSoundIndex;
	/* 현재 바닥이 페인트 위인지 확인 */
	UFUNCTION()
	bool IsOnPaintedSurface() const;

	
public:		
	/* AnimNotify에서 호출되는 발소리 재생 함수 */
	UFUNCTION()
	void PlayFootstep(EFootstepType FootstepType);
	
	/* Niagara */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	class UNiagaraComponent* PaintFx;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraSystem* PaintFx_Smoker;		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraSystem* PaintFx_NonSmoker;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	class UNiagaraComponent* StunFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	class UNiagaraComponent* DustFX;

	UFUNCTION(BlueprintCallable)
	ESurfaceType GetSurfaceTypeUnderFoot() const;
	UPROPERTY()
	APaintManager* PaintManager;
	
	UFUNCTION(BlueprintCallable)
	void OnHitEnemy(); // 외부에서 호출하게
	UFUNCTION(BlueprintCallable)
	void UseUltimate();
	UFUNCTION(Client, Reliable)
	void UpdateUIClient(int32 CurrentHits);
	
	/* !!!!!!!!!!!!!PlayerType관련!!!!!!!!!!! */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
	FLinearColor SmokerColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
	FLinearColor NonSmokerColor;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayPaintEffect();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayFireSound();
	
	UPROPERTY(ReplicatedUsing = OnRep_PlayerType)
	EPlayerType PlayerType;
	UFUNCTION()
	void OnRep_PlayerType();

	UFUNCTION()
	void SetPlayerFXColor(EPlayerType NewType);
	
	UFUNCTION(Server, Reliable)
	void SetPlayerType(EPlayerType NewType);

	/* 플레이어 타입별 정해지는 눈에 보이는 모든 것 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdatePlayerVisuals();
	UFUNCTION()
	void HidePlayerVisuals();
	
	UFUNCTION()
	void UpdateHeadIcon(EPlayerType NewType);	
	
	/* 인디케이터-미니맵 */
	UFUNCTION()
	void UpdateMinimapIcon(EPlayerType NewType);

	UFUNCTION(Server, Reliable)
	void SpawnAndAttachWeapon(EPlayerType NewType);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttachWeapon(AWeapon* SpawnedWeapon, FName SocketName);
	
	//--------------------------------------------------	
	void UpdateCharacterStat(int32 CharacterLevel);
	FCharacterSkins* GetSkinByIndex(int32 Index) const;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	FORCEINLINE FCharacterStat* GetCharacterStat() { return CharacterStat; }
	FORCEINLINE UWaeponProjectileComponent* GetWeapon() const {return Weapon;}
	FORCEINLINE FCharacterSkins* GetCharacterSkins() { return CharacterSkins; }
	
	UFUNCTION(BlueprintCallable)
	EPlayerType GetPlayerType() const { return PlayerType; }
	
	FORCEINLINE USceneCaptureComponent2D* GetFootSceneCapture(){return SceneCapture; }
	
	UPROPERTY(Replicated)
	bool bIsSmoker;

	UFUNCTION(Server, Reliable)
	void SetCanPaint(bool bEnable);
		
protected:	
	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	void SprintStart(const struct FInputActionValue& Value);
	void SprintEnd(const struct FInputActionValue& Value);
	void Interact(const struct FInputActionValue& Value);
	void MyJump(const struct FInputActionValue& Value);
	void MyStopJumping(const struct FInputActionValue& Value);
	
	UFUNCTION(Server, Reliable)
	void Interact_Server();

	/*RPC 함수*/
	UFUNCTION(Server, Reliable)
	void SprintStart_Server();
	UFUNCTION(NetMulticast, Reliable)
	void SprintStart_Client();
	
	UFUNCTION(Server, Reliable)
	void SprintEnd_Server();
	UFUNCTION(NetMulticast, Reliable)
	void SprintEnd_Client();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformAttack();
	UFUNCTION(Server, Reliable)
	void PerformAttack_Server();

	UFUNCTION(NetMulticast, Reliable)
	void PlayAttackMontage_Multicast(int32 MontageIndex);
	UFUNCTION(Server, Reliable)
	void Fire_Server();
	UFUNCTION(NetMulticast, Reliable)
	void Fire_Multicast(APaintableActor* Target, bool bIsSmoker2, FVector2D UV);
	UFUNCTION()
	void ProcessFireHit(const FHitResult& HitResult);
	UPROPERTY(Replicated)
	bool bCanPaint = false;
	
	UFUNCTION()
	void ApplyUltimateStun(AMyCharacter* Target);
	UFUNCTION(Server, Reliable)
	void ApplyUltimateStun_Server(AMyCharacter* Target);
	UFUNCTION(NetMulticast, Reliable)
	void ApplyUltimateStun_Multicast(AMyCharacter* Target);
	
protected:
	UFUNCTION()
	void OnRep_SkinChanged();
	UFUNCTION(Server, Reliable)
	void SetSkinIdx_Server(int32 Value);
	virtual void OnRep_PlayerState() override;
	UFUNCTION()
	void UpdateCharacterSkin();
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* CoughSound;
	
public:
	/*엔딩시퀀서 전용함수 */
	UFUNCTION(BlueprintCallable)
	void UpdateCharacterSkinForSeq(int32 Idx);
	
	UFUNCTION(Server, Reliable)
	void HideWeapon(EPlayerType NewType);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastHideWeapon(AWeapon* SpawnedWeapon, FName SocketName);

	UPROPERTY()
	AWeapon* CurrentWeapon;

	UFUNCTION()
	void CreateUltWidget();
	UFUNCTION()
	void RemoveUltWidget();
};






















