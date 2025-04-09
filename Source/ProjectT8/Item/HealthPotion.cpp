#include "Item/HealthPotion.h"
#include "Player/CharacterBase.h"
AHealthPotion::AHealthPotion()
{
	ItemName = "HealthPotion";
}

void AHealthPotion::Use(ACharacterBase* Player)
{
    if (HasAuthority())
    {
        if (Player)
        {
            UEffectComponent* EffectComp = Player->FindComponentByClass<UEffectComponent>();
            if (EffectComp)
            {
                FEffectParams Params;
                Params.Value = HealthRestore;
                EffectComp->ApplyEffect(EEffectType::Heal, Params);
            }
            if (AItemTestCharacter* TestPlayer = Cast<AItemTestCharacter>(Player))
            {
                TestPlayer->SetEquippedItem(nullptr);
            }
            
        }
        Destroy();
    }
    else
    {
        Server_Use(Player);
    }

}