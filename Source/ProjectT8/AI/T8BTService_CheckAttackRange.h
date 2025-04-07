#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "T8BTService_CheckAttackRange.generated.h"


UCLASS()
class PROJECTT8_API UT8BTService_CheckAttackRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	

public:
	UT8BTService_CheckAttackRange();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 200.0f;

};
