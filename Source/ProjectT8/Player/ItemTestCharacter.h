#pragma once
#include "CoreMinimal.h"
#include "Player/CharacterBase.h"
#include "EffectComponent.h" // * 추가 *
#include "ItemTestCharacter.generated.h"

class ABaseItem; //* 추가 *
UCLASS()
class PROJECTT8_API AItemTestCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	AItemTestCharacter();

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))//* 추가 * +4줄
	//UInputAction* InteractAction;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//UInputAction* UseItemAction;
	
	//void UseItem();// * 추가 * 하위 코드 전부

	///*UFUNCTION(Server, Reliable)
	//void Server_PickupItem(ABaseItem* Item);*/
	//void PickupItem(ABaseItem* Item);

	/*UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* InteractableActor);*/
	void TryInteract();

	UFUNCTION(BlueprintCallable)
	UEffectComponent* GetEffectComponent() const { return EffectComponent; }
	// void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Effect, meta = (AllowPrivateAccess = "true"))
	UEffectComponent* EffectComponent;
	/*UPROPERTY(Replicated, VisibleAnywhere)
	ABaseItem* EquippedItem;*/

public:
	void ModifyHealth(float Amount);

private:
	float Health = 100.f;
	float MaxHealth = 100.f;
};
