#include "AI/T8BTTask_UseHealItem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Item/BaseItem.h"
#include "AI/T8AICharacter.h"
#include "Player/Component/ItemComponent.h"

UT8BTTask_UseHealItem::UT8BTTask_UseHealItem()
{
	NodeName = "Use Heal Item";
	bNotifyTick = false;
}

EBTNodeResult::Type UT8BTTask_UseHealItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	AT8AICharacter* AIChar = Cast<AT8AICharacter>(AICon->GetPawn());
	if (!AIChar || !AIChar->GetItemComponent())
	{
		UE_LOG(LogTemp, Error, TEXT("[UseHealItem] AI 캐릭터 또는 ItemComponent 없음"));
		return EBTNodeResult::Failed;
	}

	UObject* HealItemObj = OwnerComp.GetBlackboardComponent()->GetValueAsObject(HealItemKey.SelectedKeyName);
	ABaseItem* HealItem = Cast<ABaseItem>(HealItemObj);
	if (!HealItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UseHealItem] 블랙보드에서 힐 아이템 가져오기 실패"));
		return EBTNodeResult::Failed;
	}

	AIChar->GetItemComponent()->TryPickUpItem(HealItem);

	OwnerComp.GetBlackboardComponent()->ClearValue(HealItemKey.SelectedKeyName);

	return EBTNodeResult::Succeeded;
}
