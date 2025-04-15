#include "AI/T8BTTask_Attack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "AI/T8AICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Player/Component/CombatComponent.h"

UT8BTTask_Attack::UT8BTTask_Attack()
{
	{
		NodeName = TEXT("Attack Target");
		bNotifyTick = false;
	}
}

EBTNodeResult::Type UT8BTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* Nodememory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	AT8AICharacter* AIChar = Cast<AT8AICharacter>(AICon->GetPawn());
	if (!AIChar || !AIChar->GetCombatComponent()) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!TargetActor) return EBTNodeResult::Failed;

	if (!AIChar->bCanAttack)
	{
		return EBTNodeResult::Failed;
	}

	AIChar->Attack();

	AIChar->bCanAttack = false;
	AIChar->bIsAttacking = true;

	AIChar->GetWorldTimerManager().SetTimer(
		AIChar->AttackCooldownTimer,
		AIChar,
		&AT8AICharacter::ResetCanAttack,
		AIChar->AttackCooldown,
		false
	);

	// Blackboard에도 IsAttacking 설정 가능
	if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
	{
		BB->SetValueAsBool(TEXT("IsAttacking"), true);
	}

	return EBTNodeResult::Succeeded;
}



