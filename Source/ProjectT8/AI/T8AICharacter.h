#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TextRenderComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GAS/CharacterAttributeSet.h"
#include "T8AICharacter.generated.h"


class ABaseItem;
class UCombatComponent;
class UGameplayEffect;
class UItemComponent;
class UCharacterAttributeSet;

UCLASS()
class PROJECTT8_API AT8AICharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AT8AICharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", Replicated)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UCharacterAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> InitEffectClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", Replicated)
	TObjectPtr<class UCombatComponent> CombatComponent;

	UFUNCTION(BlueprintCallable)
	UCombatComponent* GetCombatComponent() const { return CombatComponent; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", Replicated)
	TObjectPtr<class UItemComponent> ItemComponent;

	UFUNCTION(BlueprintCallable)
	UItemComponent* GetItemComponent() const { return ItemComponent; }


	UFUNCTION(BlueprintCallable)
	void PerformAttackHitCheck();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackCooldown = 1.0f;

	bool bCanAttack = true;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking = false;

	FTimerHandle AttackCooldownTimer;

	UFUNCTION()
	void ResetCanAttack();

	UFUNCTION(BlueprintCallable, Category = "GAS")
	float GetHealth() const;

	UPROPERTY()
	AActor* LastDamager = nullptr;

	float LastDamagerSetTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Targeting")
	float LastDamagerMemoryTime = 5.0f;

	FTimerHandle DetectionTimer;


	void Die();

	void InitAbilityActorInfo();
	void ApplyGameplayDamage(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void UseItem();
	void Attack();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	int32 TeamID = 0;

	UFUNCTION(BlueprintCallable, Category = "Team")
	bool IsEnemy(AActor* OtherActor) const;

	UPROPERTY()
	class AAIController* CachedAIController;

public:
	UFUNCTION(BlueprintCallable, Category = "Team")
	int32 GetTeamID() const;

	UFUNCTION(BlueprintCallable, Category = "Team")
	void SetTeamID(int32 NewID);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team")
	UTextRenderComponent* TeamIndicator;
};
