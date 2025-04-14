#include "AI/T8BTTask_PickupWeapon.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "Player/Component/ItemComponent.h"
#include "Item/BaseItem.h"
#include "AI/T8AICharacter.h"

UT8BTTask_PickupWeapon::UT8BTTask_PickupWeapon()
{
	NodeName = TEXT("Pick Up Nearby Weapon");

	NearbyItemKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UT8BTTask_PickupWeapon, NearbyItemKey), AActor::StaticClass());
}


EBTNodeResult::Type UT8BTTask_PickupWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	AT8AICharacter* AIChar = Cast<AT8AICharacter>(AICon->GetPawn());
	if (!AIChar || !AIChar->GetItemComponent()) return EBTNodeResult::Failed;

	UObject* TargetObj = OwnerComp.GetBlackboardComponent()->GetValueAsObject(NearbyItemKey.SelectedKeyName);
	if (!TargetObj) return EBTNodeResult::Failed;

	ABaseItem* Item = Cast<ABaseItem>(TargetObj);
	if (!Item) return EBTNodeResult::Failed;

	AIChar->GetItemComponent()->TryPickUpItem(Item);

	OwnerComp.GetBlackboardComponent()->ClearValue(NearbyItemKey.SelectedKeyName);

	return EBTNodeResult::Succeeded;
}
