#include "AI/T8BTService_CheckEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/T8AICharacter.h"
#include "Player/CharacterBase.h"

UT8BTService_CheckEnemy::UT8BTService_CheckEnemy()
{
	NodeName = "Check If Target Is Enemy";

    bNotifyBecomeRelevant = true;
    bNotifyTick = true;

    Interval = 0.5f;
    RandomDeviation = 0.1f;
}

void UT8BTService_CheckEnemy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    AAIController* Controller = OwnerComp.GetAIOwner();
    AT8AICharacter* SelfAI = Cast<AT8AICharacter>(Controller ? Controller->GetPawn() : nullptr);

    if (!Blackboard || !SelfAI)
    {
        return;
    }

    AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject("Target"));
    if (!Target)
    {
        return;
    }

    if (AT8AICharacter* TargetAI = Cast<AT8AICharacter>(Target))
    {
        if (TargetAI->IsDead())
        {
            return;
        }
    }

    if (!SelfAI->IsEnemy(Target))
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[CheckEnemy] 유지됨: %s → %s"), *SelfAI->GetName(), *Target->GetName());
}



