#include "Item/PoisonKnife.h"

APoisonKnife::APoisonKnife()
{
    ItemName = "PoisonKnife";
    ThrowDuration = 0.0f;
}

void APoisonKnife::ApplyEffect(ACharacterBase* Target)
{
    if (Target)
    {
        UEffectComponent* EffectComp = Target->FindComponentByClass<UEffectComponent>();
        if (EffectComp)
        {
            FEffectParams Params;
            Params.Duration = 5.0f;
            EffectComp->ApplyEffect(EEffectType::Poison, Params);
        }
    }
}