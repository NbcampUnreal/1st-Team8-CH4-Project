#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "T8BTTask_Attack.generated.h"


UCLASS()
class PROJECTT8_API UT8BTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
	UT8BTTask_Attack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackDelay = 1.3f;
};
