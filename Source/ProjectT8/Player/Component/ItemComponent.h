#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemComponent.generated.h"

class ABaseItem;
class ACharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTT8_API UItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UItemComponent();

	void Init(ACharacter* InOwner);

	void TryPickUpItem(ABaseItem* NewItem);
	void UseEquippedItem();
	void DropItemToWorld();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AttachItem(ABaseItem* Item);

	UFUNCTION(BlueprintCallable)
	ABaseItem* GetEquippedItem() const { return EquippedItem; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	UPROPERTY(Replicated)
	ABaseItem* EquippedItem;

	UPROPERTY()
	ACharacter* OwnerCharacter;

	UPROPERTY(EditAnywhere, Category = "Item")
	FVector DropOffset = FVector(30.f, 0.f, 50.f);
};
