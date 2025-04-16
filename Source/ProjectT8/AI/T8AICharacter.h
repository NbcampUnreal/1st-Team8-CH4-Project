#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GAS/CharacterAttributeSet.h"
#include "T8AICharacter.generated.h"


// AI 전용 죽음 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIDeathDelegate, AT8AICharacter*, DeadAI);



class ABaseItem;
class UCombatComponent;
class UGameplayEffect;
class UItemComponent;
class UCharacterAttributeSet;
class UFloatingStatusWidget;

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

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth() const;

	UFUNCTION()
	void HandleHealthChanged(float NewHealth, float MaxHealth);

	UPROPERTY()
	AActor* LastDamager = nullptr;

	float LastDamagerSetTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Targeting")
	float LastDamagerMemoryTime = 5.0f;

	FTimerHandle DetectionTimer;

	UPROPERTY(BlueprintAssignable)
	FAIDeathDelegate OnCharacterDeath;


	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
	bool bIsDead = false;

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void Die();

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastDie();

	UFUNCTION(BlueprintPure, Category = "Character")
	virtual bool IsDead() const { return bIsDead; }

	void InitAbilityActorInfo();
	void ApplyGameplayDamage(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void UseItem();
	void Attack();

	UFUNCTION(BlueprintCallable, Category = "Team")
	bool IsEnemy(AActor* OtherActor) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	int32 TeamID = 0;

	

	UPROPERTY()
	class AAIController* CachedAIController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* FloatingStatusWidget;

	UPROPERTY()
	UFloatingStatusWidget* StatusWidget;

	void InitializeFloatingStatusWidget();
	void UpdateHealthUI();

public:
	UFUNCTION(BlueprintCallable, Category = "Team")
	int32 GetTeamID() const;

	/*UFUNCTION(BlueprintCallable, Category = "Team")
	void SetTeamID(int32 NewID);*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team")
	UTextRenderComponent* TeamIndicator;
};
