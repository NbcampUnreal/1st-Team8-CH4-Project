#include "Player/AnimNotifyDealDamage.h"
#include "CharacterBase.h"
#include "Player/CombatComponent.h"

void UAnimNotifyDealDamage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	if (ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (UCombatComponent* Combat = Character->GetCombatComponent())
		{
			Combat->HandleAttackNotify();
		}
	}
}
