#include "Item/Melee.h"

AMelee::AMelee()
{
	WeaponType = EWeaponType::Melee;
	ItemName = "Melee";
}
void AMelee::Use(ACharacterBase* Player)
{	// 공격 로직
	TArray<FHitResult> HitResults;
	//주변 액터 감지
	// TArray<FHitResult>는 충돌 검사(Trace/Sweep)에서 여러 충돌 결과를 저장하는 배열
	// LineTraceMulti 또는 SweepMulti 같은 함수와 함께 사용
	// 
	//플레이어액터->takedamage
}