#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "BaseItem.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon,
	Throwable,
	Consumable
};
class ATestCharacter;
UCLASS()
class PROJECTT8_API ABaseItem : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	ABaseItem();
	virtual void Interact_Implementation(ATestCharacter* Player) override;
	virtual void Use(ATestCharacter* Player);

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType;
};