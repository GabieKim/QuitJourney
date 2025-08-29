// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MainMenuPawn.generated.h"

UCLASS()
class MULTITEST2_API AMainMenuPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Arrow, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UArrowComponent> Arrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> SMesh;
	
public:
	AMainMenuPawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData, meta = (AllowPrivateAccess = "true"))
	class UDataTable* CharacterSkinsDataTable;
	
	struct FCharacterSkins* CharacterSkin;
	
protected:
	/* 서버에서 랜덤 스킨을 정하는 함수 */
	UFUNCTION(Server, Reliable)
	void Server_RandomizeCharacterSkin();
	void Server_RandomizeCharacterSkin_Implementation();
	/* 모든 클라이언트에게 스킨을 전파하는 함수 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RandomizeCharacterSkin(int32 NewIdx);
	void Multicast_RandomizeCharacterSkin_Implementation(int32 NewIdx);

public:
	UFUNCTION(BlueprintCallable)
	void ChangeSkinIndex(int32 Direction);
	

	UFUNCTION(BlueprintCallable)
	void UpdateSkinPreview(int32 NewIdx);
	UFUNCTION(BlueprintCallable)
	void ApplyCharacterSkin(int32 NewIdx);
	UFUNCTION(BlueprintCallable)
	void SetCharacterMaterials();

	FCharacterSkins* GetSkinByIndex(int32 Index) const;
	int32 GetTotalSkinCount() const;

	
protected:
	/* UI에서 `Confirm` 버튼을 눌렀을 때 실행 → 서버에 스킨 저장 요청 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ApplyCharacterSkin(int32 NewIdx);
	void Server_ApplyCharacterSkin_Implementation(int32 NewIdx);
	bool Server_ApplyCharacterSkin_Validate(int32 NewIdx) { return true; }
	/* 서버에서 선택한 스킨을 모든 클라이언트에 반영 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyCharacterSkin(int32 NewIdx);
	void Multicast_ApplyCharacterSkin_Implementation(int32 NewIdx);

	/* 현재 선택된 스킨 인덱스 */
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentSkinIdx;
	
public: 
	FORCEINLINE class UArrowComponent* GetArrow() const { return Arrow; }
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMesh() const { return SMesh; }
	FORCEINLINE int32 GetCurrentSkinIdx() const { return CurrentSkinIdx; }
	FORCEINLINE FCharacterSkins* GetCharacterSkins() const { return CharacterSkin;}
};






