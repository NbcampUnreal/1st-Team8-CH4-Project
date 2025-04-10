#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "CharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class ABaseItem;
class UCombatComponent;
class UAbilitySystemComponent;
class UCharacterAttributeSet;
class UGameplayEffect;
struct FInputActionValue;

UCLASS()
class ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();

	// GAS
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Ability System Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", Replicated)
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UCharacterAttributeSet> AttributeSet;

	// Combat
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<class UCombatComponent> CombatComponent;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> StunEffectClass;

	TArray<FGameplayTag> StatusEffectTags;
	void RegisterStatusEffectDelegates();
	UFUNCTION()
	void OnStatusEffectTagChanged(const FGameplayTag Tag, int32 NewCount);

	void ShowStatusWidget(const FGameplayTag& Tag);
	void HideStatusWidget(const FGameplayTag& Tag);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> BurnWidgetClass;

	UPROPERTY()
	UUserWidget* BurnWidgetInstance;

	void InitAbilityActorInfo();
	void ApplyGameplayEffectToTarget(ACharacterBase* Target, TSubclassOf<UGameplayEffect> EffectClass);
	void OnAttackHit();
	void DealDamageToActors(const TArray<FHitResult>& HitResults);
	void ApplyKnockback(AActor* TargetActor);

	bool CanAttack() const;

	// RPC
	UFUNCTION(Server, Reliable)
	void Server_Attack();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAttackMontage();

	UFUNCTION(Server, Reliable)
	void Server_ApplyEffectToTarget(ACharacterBase* Target, TSubclassOf<UGameplayEffect> EffectClass);

	UFUNCTION(Server, Reliable)
	void Server_ApplyKnockback(AActor* TargetActor);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyKnockback(AActor* TargetActor, FVector Direction);

	// Item
	UPROPERTY(Replicated)
	ABaseItem* EquippedItem;

	UFUNCTION(BlueprintCallable)
	void PickupItem(ABaseItem* Item);

	UFUNCTION(BlueprintCallable)
	void UseItem();

	UFUNCTION(BlueprintCallable)
	void TryInteract();

	UFUNCTION(Server, Reliable)
	void Server_PickupItem(ABaseItem* Item);

	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* InteractableActor);

	// Combat
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UGameplayEffect> CurrentDamageEffect;

protected:
	// Input
	virtual void BeginPlay() override;
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void SprintStart();
	void SprintEnd();
	void Attack();

	// Input Mapping
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AttackAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* InteractAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* UseItemAction;


	// Camera
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	

private:
	bool bIsRunning = false;

	const float WalkSpeed = 150.f;
	const float RunSpeed = 600.f;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};