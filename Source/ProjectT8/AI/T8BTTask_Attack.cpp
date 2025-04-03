#include "AI/T8BTTask_Attack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"


UT8BTTask_Attack::UT8BTTask_Attack()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UT8BTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* Nodememory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	ACharacter* AIPawn = Cast<ACharacter>(AICon->GetPawn());
	if (!AIPawn) return EBTNodeResult::Failed;

	UAnimInstance* AnimInstance = AIPawn->GetMesh()->GetAnimInstance();
	if (AttackMontage && AnimInstance)
	{
		AnimInstance->Montage_Play(AttackMontage);
		UE_LOG(LogTemp, Warning, TEXT("AI is attacking."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AI tried to attack."));
	}

	return EBTNodeResult::Succeeded;
}



