#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "T8BTTask_PickupWeapon.generated.h"


UCLASS()
class PROJECTT8_API UT8BTTask_PickupWeapon : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
	UT8BTTask_PickupWeapon();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector NearbyItemKey;
};
