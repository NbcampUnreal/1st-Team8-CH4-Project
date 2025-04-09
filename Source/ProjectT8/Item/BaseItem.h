#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Components/SphereComponent.h"
#include "Player/CharacterBase.h"
#include "Player/ItemTestCharacter.h"
#include "BaseItem.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon,
	Throwable,
	Consumable
};
class AItemTestCharacter;

UCLASS()
class PROJECTT8_API ABaseItem : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	ABaseItem();
	virtual void Interact_Implementation(ACharacterBase* Player) override;
	virtual void Use(ACharacterBase* Player);

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect> AssociatedEffect;

	UFUNCTION(BlueprintCallable)
	virtual TSubclassOf<UGameplayEffect> GetAssociatedGameplayEffect() const { return AssociatedEffect; }

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RootComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* InteractSphere;
};