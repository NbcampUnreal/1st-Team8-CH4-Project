#include "T8BTTask_UseWeapon.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Player/Component/ItemComponent.h"
#include "Item/Weapon.h"
#include "Player/CharacterBase.h"

UT8BTTask_UseWeapon::UT8BTTask_UseWeapon()
{
	NodeName = TEXT("Use Equipped Weapon");
}

EBTNodeResult::Type UT8BTTask_UseWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (!AIController) return EBTNodeResult::Failed;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	UItemComponent* ItemComp = AIPawn->FindComponentByClass<UItemComponent>();
	if (!ItemComp) return EBTNodeResult::Failed;

	ABaseItem* EquippedItem = ItemComp->GetEquippedItem();
	if (!EquippedItem) return EBTNodeResult::Failed;

	// 현재 구조에서는 ACharacterBase*로 캐스팅해야만 무기 Use()가 정상 작동함
	ACharacterBase* AsPlayer = Cast<ACharacterBase>(AIPawn);
	if (!AsPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_UseWeapon 캐스팅 실패: AI 캐릭터는 현재 Use()를 실행할 수 없음"));
		return EBTNodeResult::Succeeded; // 실패로 처리하면 BT 멈춤 → 일단 성공 처리
	}

	UE_LOG(LogTemp, Warning, TEXT("BTTask_UseWeapon Use() 호출: %s"), *EquippedItem->GetName());
	EquippedItem->Use(AsPlayer);

	/*무기 Use 함수가 AActor*로 바뀌면 활성화
	EquippedItem->Use(AIPawn);*/

	return EBTNodeResult::Succeeded;
}
