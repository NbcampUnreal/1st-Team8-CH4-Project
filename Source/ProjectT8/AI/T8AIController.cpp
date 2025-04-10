#include "AI/T8AIController.h"
#include "AI/T8AICharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

AT8AIController::AT8AIController()
{
}

void AT8AIController::BeginPlay()
{
	Super::BeginPlay();

	if (UseBlackboard(BehaviorTreeAsset->BlackboardAsset, Blackboard))
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AT8AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AT8AICharacter* T8Char = Cast<AT8AICharacter>(GetPawn());
	if (T8Char && T8Char->bIsAttacking)
	{
		//StopMovement();
	}
}

AActor* AT8AIController::GetTargetPlayer() const
{
	if (GetBlackboardComponent())
	{
		return Cast<AActor>(GetBlackboardComponent()->GetValueAsObject("TargetPlayer"));
	}

	return nullptr;
}

