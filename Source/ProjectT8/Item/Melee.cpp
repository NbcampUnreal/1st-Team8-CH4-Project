#include "Item/Melee.h"

AMelee::AMelee()
{
	WeaponType = EWeaponType::Melee;
	ItemName = "Melee";
}
void AMelee::Attack()
{	// ���� ����
	TArray<FHitResult> HitResults;
	//�ֺ� ���� ����
	// TArray<FHitResult>�� �浹 �˻�(Trace/Sweep)���� ���� �浹 ����� �����ϴ� �迭
	// LineTraceMulti �Ǵ� SweepMulti ���� �Լ��� �Բ� ���
	// 
	//�÷��̾����->takedamage
}