#include "Item/BlackHoleGenerator.h"

ABlackHoleGenerator::ABlackHoleGenerator()
{
    ItemName = "BlackHoleGenerator";
    ThrowDuration = 2.0f;
}

void ABlackHoleGenerator::ApplyEffect(ACharacterBase* Target)
{
    if (Target)
    {
        UEffectComponent* EffectComp = Target->FindComponentByClass<UEffectComponent>();
        if (EffectComp)
        {
            FEffectParams Params;
            EffectComp->ApplyEffect(EEffectType::Teleport, Params);
        }
    }
}
