#include "AnimNotify_DealDamage_AI.h"
#include "AI/T8AICharacter.h"
#include "Engine/Engine.h"

void UAnimNotify_DealDamage_AI::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UE_LOG(LogTemp, Warning, TEXT("Notify 호출됨 - DealDamage_AI"));

	if (!MeshComp) return;

	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (AT8AICharacter* AIChar = Cast<AT8AICharacter>(Owner))
		{
			AIChar->PerformAttackHitCheck();
		}
	}
}
