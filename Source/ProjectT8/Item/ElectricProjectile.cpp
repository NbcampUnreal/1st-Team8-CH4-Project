#include "Item/ElectricProjectile.h"

AElectricProjectile::AElectricProjectile()
{
}

void AElectricProjectile::ApplyEffect(ACharacterBase* Target)
{
    if (Target)
    {
        UEffectComponent* EffectComp = Target->FindComponentByClass<UEffectComponent>();
        if (EffectComp)
        {
            FEffectParams Params;
            Params.Duration = 3.0f; // 3�ʰ� ���� ȿ��
            EffectComp->ApplyEffect(EEffectType::Electric, Params);
        }
    }
}