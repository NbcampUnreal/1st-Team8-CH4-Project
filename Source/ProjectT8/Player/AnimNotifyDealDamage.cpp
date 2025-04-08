#include "Player/AnimNotifyDealDamage.h"
#include "CharacterBase.h"

void UAnimNotifyDealDamage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	ACharacterBase* OwnerChar = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (OwnerChar)
	{
		OwnerChar->OnAttackHit();
	}
}
