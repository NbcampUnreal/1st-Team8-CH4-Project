#include "Item/SpeedBoostPotion.h"

ASpeedBoostPotion::ASpeedBoostPotion()
{
	ItemName = "SpeedBoostPotion";
}

void ASpeedBoostPotion::Use(ACharacterBase* Player)
{
    if (Player)
    {
        UEffectComponent* EffectComp = Player->FindComponentByClass<UEffectComponent>();
        if (EffectComp)
        {
            FEffectParams Params;
            Params.Multiplier = 1.5f; // �ӵ� 1.5��
            Params.Duration = 5.0f;   // 5�� ����
            EffectComp->ApplyEffect(EEffectType::SpeedUp, Params);
        }
	    Destroy();
    }
}