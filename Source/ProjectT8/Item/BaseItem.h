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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_Use(ACharacterBase* Player);
	virtual void Use(ACharacterBase* Player);
	UStaticMeshComponent* GetItemMesh() const { return ItemMesh; }

	UPROPERTY(ReplicatedUsing = OnRep_IsPickedUp)
	bool bIsPickedUp;
	UFUNCTION()
	void OnRep_IsPickedUp();

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