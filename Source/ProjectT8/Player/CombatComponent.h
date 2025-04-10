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

	void ApplyGameplayEffectToTarget(ACharacterBase* Target, TSubclassOf<UGameplayEffect> EffectClass);
	void ApplyKnockback(AActor* TargetActor);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAttackMontage();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyKnockback(AActor* TargetActor, FVector Direction);

	UFUNCTION(Server, Reliable)
	void Server_Attack();
	UFUNCTION(Server, Reliable)
	void Server_ApplyKnockback(AActor* TargetActor);
	UFUNCTION(Server, Reliable)
	void Server_ApplyEffectToTarget(ACharacterBase* Target, TSubclassOf<UGameplayEffect> EffectClass);

	UFUNCTION(BlueprintCallable)
	void HandleAttackNotify();

private:
	UPROPERTY()
	ACharacterBase* OwnerCharacter;
};
