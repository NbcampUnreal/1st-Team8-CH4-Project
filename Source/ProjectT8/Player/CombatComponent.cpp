#include "Player/CombatComponent.h"
#include "CharacterBase.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UCombatComponent::Init(ACharacterBase* InOwner)
{
	OwnerCharacter = InOwner;
}

void UCombatComponent::Attack()
{
	if (!OwnerCharacter || !OwnerCharacter->CanAttack()) return;

	if (!OwnerCharacter->HasAuthority())
	{
		Server_Attack();
		return;
	}

	Multicast_PlayAttackMontage();
}

void UCombatComponent::Server_Attack_Implementation()
{
	Multicast_PlayAttackMontage();
}

void UCombatComponent::Multicast_PlayAttackMontage_Implementation()
{
	if (!OwnerCharacter || !OwnerCharacter->AttackMontage) return;

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance && !AnimInstance->Montage_IsPlaying(OwnerCharacter->AttackMontage))
	{
		AnimInstance->Montage_Play(OwnerCharacter->AttackMontage);
	}
}

void UCombatComponent::HandleAttackNotify()
{
	OnAttackHit();
}

void UCombatComponent::OnAttackHit()
{
	if (!OwnerCharacter) return;

	FVector Start = OwnerCharacter->GetActorLocation();
	FVector Forward = OwnerCharacter->GetActorForwardVector();
	FVector End = Start + Forward * 150.f;
	TArray<FHitResult> HitResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(60.f);

	bool bHit = OwnerCharacter->GetWorld()->SweepMultiByChannel(
		HitResults, Start, End, FQuat::Identity, ECC_Pawn, Sphere);

	if (bHit)
	{
		DealDamageToActors(HitResults);
	}
}

void UCombatComponent::DealDamageToActors(const TArray<FHitResult>& HitResults)
{
	for (const FHitResult& Hit : HitResults)
	{
		ACharacterBase* TargetCharacter = Cast<ACharacterBase>(Hit.GetActor());
		if (TargetCharacter && TargetCharacter != OwnerCharacter)
		{
			UE_LOG(LogTemp, Warning, TEXT("타격 대상: %s"), *TargetCharacter->GetName());

			if (OwnerCharacter->HasAuthority())
			{
				if (OwnerCharacter->CurrentDamageEffect)
				{
					ApplyGameplayEffectToTarget(TargetCharacter, OwnerCharacter->CurrentDamageEffect);
				}
				ApplyKnockback(TargetCharacter);
			}
			else
			{
				if (OwnerCharacter->CurrentDamageEffect)
				{
					Server_ApplyEffectToTarget(TargetCharacter, OwnerCharacter->CurrentDamageEffect);
				}
				Server_ApplyKnockback(TargetCharacter);
			}
		}
	}
}

void UCombatComponent::ApplyGameplayEffectToTarget(ACharacterBase* Target, TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!OwnerCharacter || !EffectClass || !Target) return;
	UAbilitySystemComponent* ASC = OwnerCharacter->GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(OwnerCharacter);
	
	FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(EffectClass, 1.0f, Context);
	if (Spec.IsValid())
	{
		Target->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		UE_LOG(LogTemp, Warning, TEXT("%s 에게 이펙트 적용: %s"), *Target->GetName(), *EffectClass->GetName());
	}
}

void UCombatComponent::ApplyKnockback(AActor* TargetActor)
{	
	if (!OwnerCharacter || !OwnerCharacter->HasAuthority()) return;

	ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	if (!TargetCharacter) return;

	FVector KnockbackDir = (TargetCharacter->GetActorLocation() - OwnerCharacter->GetActorLocation()).GetSafeNormal();
	KnockbackDir.Z = 0.5f;
	KnockbackDir.Normalize();
	Multicast_ApplyKnockback(TargetActor, KnockbackDir);
}

void UCombatComponent::Multicast_ApplyKnockback_Implementation(AActor* TargetActor, FVector Direction)
{
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	if (!TargetCharacter) return;

	const float KnockbackStrength = 800.f;
	FVector knockbackForce = Direction * KnockbackStrength;
	TargetCharacter->LaunchCharacter(knockbackForce, true, true);
}

void UCombatComponent::Server_ApplyEffectToTarget_Implementation(ACharacterBase* Target, TSubclassOf<UGameplayEffect> EffectClass)
{
	ApplyGameplayEffectToTarget(Target, EffectClass);
}

void UCombatComponent::Server_ApplyKnockback_Implementation(AActor* TargetActor)
{
	ApplyKnockback(TargetActor);
}