#include "AI/T8AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"

AT8AIController::AT8AIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    SetPerceptionComponent(*AIPerception);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	if (SightConfig)
	{
		SightConfig->SightRadius = 2000.0f;
		SightConfig->LoseSightRadius = 2200.0f;
		SightConfig->PeripheralVisionAngleDegrees = 90.0f;
		SightConfig->SetMaxAge(5.0f);
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		AIPerception->ConfigureSense(*SightConfig);
		AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	}

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AT8AIController::OnTargetPerceived);
}

void AT8AIController::BeginPlay()
{
	Super::BeginPlay();

	if (UseBlackboard(BehaviorTreeAsset->BlackboardAsset, Blackboard))
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AT8AIController::OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("Target Sensed: %s"), *Actor->GetName());
		GetBlackboardComponent()->SetValueAsObject("TargetActor", Actor);
		//이후에 Blackboard에 TargetActor 세팅 예정
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Target Lost: %s"), *Actor->GetName());
		GetBlackboardComponent()->ClearValue("TargetActor");
		//마찬가지
	}
}

