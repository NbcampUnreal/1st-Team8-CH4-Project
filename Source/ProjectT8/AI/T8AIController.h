#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "T8AIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class PROJECTT8_API AT8AIController : public AAIController
{
	GENERATED_BODY()
	

public:
	AT8AIController();

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTargetPlayer() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

private:
	UBlackboardComponent* Blackboard;

	virtual void Tick(float DeltaTime) override;
};
