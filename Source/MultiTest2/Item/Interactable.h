#pragma once
#include "UObject/Interface.h"
#include "Interactable.generated.h"

// 실제 인터페이스 아니고, 언리얼에서 클래스 볼 수 있게 하는 빈 클래스
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

// 실제 인터페이스
class MULTITEST2_API IInteractable	
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction", meta = (DisplayName = "Interact"))
	void Interact(class AMyCharacter* CharacterInstigator);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction", meta = (DisplayName = "Interact"))
	bool CanInteract(AMyCharacter* CharacterInstigator)const;
};
