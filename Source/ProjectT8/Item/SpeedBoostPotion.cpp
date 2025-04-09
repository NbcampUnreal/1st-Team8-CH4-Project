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
            Params.Multiplier = 1.5f; // 속도 1.5배
            Params.Duration = 5.0f;   // 5초 지속
            EffectComp->ApplyEffect(EEffectType::SpeedUp, Params);
        }
	    Destroy();
    }
}