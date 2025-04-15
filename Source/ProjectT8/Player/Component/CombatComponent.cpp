#include "Player/Component/CombatComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/CharacterBase.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UCombatComponent::Init(ACharacter* InOwner)
{
	OwnerCharacter = InOwner;
}

void UCombatComponent::Attack()
{
	if (!OwnerCharacter || !CanAttack()) return;

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
	if (!OwnerCharacter || !AttackMontage) return;

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance && !AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		AnimInstance->Montage_Play(AttackMontage);
	}
}

void UCombatComponent::HandleAttackNotify()
{
	OnAttackHit();
}

bool UCombatComponent::CanAttack() const
{
	if (ACharacterBase* CharBase = Cast<ACharacterBase>(OwnerCharacter))
	{
		return !CharBase->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Stunned"));
	}
	return true;
}

void UCombatComponent::OnAttackHit()
{
	if (!OwnerCharacter) return;

	FVector Start = OwnerCharacter->GetActorLocation();
	FVector Forward = OwnerCharacter->GetActorForwardVector();
	FVector End = Start + Forward * 150.f;
	TArray<FHitResult> HitResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(60.f);

	UWorld* World = OwnerCharacter->GetWorld();
	if (!World) return;

	bool bHit = World->SweepMultiByChannel(
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
		if (ACharacterBase* TargetCharacter = Cast<ACharacterBase>(Hit.GetActor()))
		{
			if (TargetCharacter != OwnerCharacter)
			{
				UE_LOG(LogTemp, Warning, TEXT("타격 대상: %s"), *TargetCharacter->GetName());

				if (OwnerCharacter->HasAuthority())
				{
					if (CurrentDamageEffect)
					{
						ApplyGameplayEffectToTarget(TargetCharacter, CurrentDamageEffect);
					}
					ApplyKnockback(TargetCharacter);
				}
				else
				{
					if (CurrentDamageEffect)
					{
						Server_ApplyEffectToTarget(TargetCharacter, CurrentDamageEffect);
					}
					Server_ApplyKnockback(TargetCharacter);
				}
			}
		}
	}
}

void UCombatComponent::ApplyGameplayEffectToTarget(ACharacter* Target, TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!OwnerCharacter || !EffectClass || !Target) return;
	UAbilitySystemComponent* ASC = Cast<ACharacterBase>(Target)->GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(Target);
	
	FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(EffectClass, 1.0f, Context);
	if (Spec.IsValid())
	{
		Cast<ACharacterBase>(Target)->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
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

	FVector knockbackForce = Direction * KnockbackStrength;
	TargetCharacter->LaunchCharacter(knockbackForce, true, true);
}

void UCombatComponent::Server_ApplyEffectToTarget_Implementation(ACharacter* Target, TSubclassOf<UGameplayEffect> EffectClass)
{
	ApplyGameplayEffectToTarget(Target, EffectClass);
}

void UCombatComponent::Server_ApplyKnockback_Implementation(AActor* TargetActor)
{
	ApplyKnockback(TargetActor);
}