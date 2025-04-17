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
		Blackboard->SetValueAsBool("Target", false);
		return;
	}

	if (ACharacterBase* TargetChar = Cast<ACharacterBase>(Target))
	{
		if (TargetChar->IsDead())
		{
			Blackboard->SetValueAsBool("Target", false);
			return;
		}
	}
	else if (AT8AICharacter* TargetAI = Cast<AT8AICharacter>(Target))
	{
		if (TargetAI->IsDead())
		{
			Blackboard->SetValueAsBool("Target", false);
			return;
		}
	}

	bool bIsEnemy = SelfAI->IsEnemy(Target);
	Blackboard->SetValueAsBool("Target", bIsEnemy);

	UE_LOG(LogTemp, Warning, TEXT("[BTService] %s → Target: %s → IsEnemy: %s"),
		*SelfAI->GetName(),
		*Target->GetName(),
		bIsEnemy ? TEXT("True") : TEXT("False"));
}

