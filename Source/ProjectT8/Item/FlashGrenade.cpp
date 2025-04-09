#include "Item/FlashGrenade.h"

AFlashGrenade::AFlashGrenade()
{
	ItemName = "FlashGrenade";
	ThrowDuration = 2.0f;
}

void AFlashGrenade::ApplyEffect(ACharacterBase* Target)
{
    if (Target)
    {
        UEffectComponent* EffectComp = Target->FindComponentByClass<UEffectComponent>();
        if (EffectComp)
        {
            FEffectParams Params;
            Params.Duration = 2.0f;
            EffectComp->ApplyEffect(EEffectType::Flash, Params);
        }
    }
}