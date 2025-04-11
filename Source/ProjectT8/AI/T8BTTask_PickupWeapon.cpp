#include "AI/T8BTTask_PickupWeapon.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "AI/T8AICharacter.h"

UT8BTTask_PickupWeapon::UT8BTTask_PickupWeapon()
{
	NodeName = "Pickup Weapon";
}


EBTNodeResult::Type UT8BTTask_PickupWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	AT8AICharacter* AIChar = Cast<AT8AICharacter>(AICon->GetPawn());
	if (!AIChar) return EBTNodeResult::Failed;

	auto* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* WeaponActor = Cast<AActor>(BB->GetValueAsObject(TargetWeaponKey.SelectedKeyName));
	if (!WeaponActor) return EBTNodeResult::Failed;

	//예시
	/*if (auto* ItemComp = AIChar->FindComponentByClass<UItemComponent>())
	{
		ItemComp->Pickup(WeaponActor);
		BB->SetValueAsBool(HasWeaponKey.SelectedKeyName, true);
		return EBTNodeResult::Succeeded;
	}*/

	//임시처리
	UE_LOG(LogTemp, Warning, TEXT("무기 줍기 로직은 추후 컴포넌트 연동 후 구현 예정"));
	return EBTNodeResult::Failed;
}
