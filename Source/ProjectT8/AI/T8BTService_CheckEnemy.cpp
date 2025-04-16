#include "AI/T8BTService_CheckEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/T8AICharacter.h"
#include "Player/CharacterBase.h"

UT8BTService_CheckEnemy::UT8BTService_CheckEnemy()
{
	NodeName = "Check If Target Is Enemy";
	Interval = 0.5f;
}

void UT8BTService_CheckEnemy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	auto* Blackboard = OwnerComp.GetBlackboardComponent();
	auto* Controller = OwnerComp.GetAIOwner();
	auto* SelfAI = Cast<AT8AICharacter>(Controller ? Controller->GetPawn() : nullptr);
	auto* Target = Cast<AActor>(Blackboard->GetValueAsObject("Target"));

	if (!SelfAI || !Target)
	{
		Blackboard->SetValueAsBool("IsTargetEnemy", false);
		return;
	}

	bool bIsEnemy = SelfAI->IsEnemy(Target);
	Blackboard->SetValueAsBool("IsTargetEnemy", bIsEnemy);
}
