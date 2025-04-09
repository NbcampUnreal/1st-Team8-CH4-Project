#include "AI/T8BTService_CheckAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Actor.h"


UT8BTService_CheckAttackRange::UT8BTService_CheckAttackRange()
{
	NodeName = TEXT("Check Attack Range");
	bNotifyBecomeRelevant = true;
	bNotifyTick = true;
}

void UT8BTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(GetSelectedBlackboardKey()));
	AAIController* AICon = OwnerComp.GetAIOwner();
	APawn* AIPawn = AICon ? AICon->GetPawn() : nullptr;

	if (Target && AIPawn)
	{
		const float Distance = FVector::Dist(Target->GetActorLocation(), AIPawn->GetActorLocation());
		const bool bInRange = Distance <= AttackRange;

		BlackboardComp->SetValueAsBool(TEXT("IsInAttackRange"), bInRange);

		UE_LOG(LogTemp, Warning, TEXT("Distance = %.1f / InRange = %s"), Distance, bInRange ? TEXT("true") : TEXT("false"));
	}
}