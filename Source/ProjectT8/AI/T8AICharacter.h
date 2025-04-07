#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "T8AICharacter.generated.h"

UCLASS()
class PROJECTT8_API AT8AICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AT8AICharacter();

	virtual void BeginPlay() override;

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

protected:
	
};
