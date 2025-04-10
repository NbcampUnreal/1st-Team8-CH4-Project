#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TextRenderComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GAS/CharacterAttributeSet.h"
#include "T8AICharacter.generated.h"


UCLASS()
class PROJECTT8_API AT8AICharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AT8AICharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UCharacterAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> InitEffectClass;


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

	FTimerHandle DetectionTimer;

	UFUNCTION()
	void DetectNearbyActors();

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHP = 100.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHP = 0.0f;
	UFUNCTION(BlueprintCallable)
	void ApplyDamage(float DamageAmount);*/
	void Die();

	UPROPERTY()
	AActor* CurrentTarget = nullptr;
	float TargetTrackTime = 0.0f;
	AActor* PotentialTarget = nullptr;
	float PotentialTargetTime = 0.0f;
	const float TargetStickTime = 2.0f;

	void InitAbilityActorInfo();
	void ApplyGameplayDamage(AActor* TargetActor);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	int32 TeamID = 0;

	UFUNCTION(BlueprintCallable, Category = "Team")
	bool IsEnemy(AActor* OtherActor) const;

public:
	UFUNCTION(BlueprintCallable, Category = "Team")
	int32 GetTeamID() const;

	UFUNCTION(BlueprintCallable, Category = "Team")
	void SetTeamID(int32 NewID);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team")
	UTextRenderComponent* TeamIndicator;
};
