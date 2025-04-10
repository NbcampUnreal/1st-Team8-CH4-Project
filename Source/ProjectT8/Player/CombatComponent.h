#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "CombatComponent.generated.h"

class ACharacterBase;
class UGameplayEffect;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTT8_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	void Init(ACharacterBase* InOwner);

	void Attack();
	void OnAttackHit();
	void DealDamageToActors(const TArray<FHitResult>& HitResults);

	void ApplyKnockback(AActor* TargetActor);
	void ApplyGameplayEffectToTarget(ACharacterBase* Target, TSubclassOf<UGameplayEffect> EffectClass);

	void Server_Attack();
	void Multicast_PlayAttackMontage();

	void Server_ApplyEffectToTarget(ACharacterBase* Target, TSubclassOf<UGameplayEffect> EffectClass);
	void Server_ApplyKnockback(AActor* TargetActor);
	void Multicast_ApplyKnockback(AActor* TargetActor, FVector Direction);

private:
	UPROPERTY()
	ACharacterBase* OwnerCharacter;
};
