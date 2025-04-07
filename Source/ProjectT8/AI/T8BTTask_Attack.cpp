#include "AI/T8BTTask_Attack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "AI/T8AICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

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
	AT8AICharacter* T8Char = Cast<AT8AICharacter>(AIPawn);

	if (T8Char && T8Char->AttackMontage && AnimInstance)
	{
		if (!T8Char->bCanAttack)
		{
			UE_LOG(LogTemp, Warning, TEXT("Attack on cooldown."));
			return EBTNodeResult::Failed;
		}

		T8Char->bCanAttack = false;
		T8Char->bIsAttacking = true;

		AICon->GetBlackboardComponent()->SetValueAsBool(TEXT("IsAttacking"), true);

		AnimInstance->Montage_Play(T8Char->AttackMontage);
		UE_LOG(LogTemp, Warning, TEXT("AI is attacking."));

		T8Char->GetWorldTimerManager().SetTimer(
			T8Char->AttackCooldownTimer,
			T8Char,
			&AT8AICharacter::ResetCanAttack,
			T8Char->AttackCooldown,
			false
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AI tried to attack."));
	}

	return EBTNodeResult::Succeeded;
}



