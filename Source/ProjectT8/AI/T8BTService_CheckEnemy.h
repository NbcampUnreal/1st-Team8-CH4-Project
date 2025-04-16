#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "T8BTService_CheckEnemy.generated.h"

UCLASS()
class PROJECTT8_API UT8BTService_CheckEnemy : public UBTService
{
	GENERATED_BODY()

public:
	UT8BTService_CheckEnemy();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
