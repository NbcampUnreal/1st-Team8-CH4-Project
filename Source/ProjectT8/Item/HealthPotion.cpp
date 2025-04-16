#include "Item/HealthPotion.h"
#include "Player/CharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
AHealthPotion::AHealthPotion()
{
	ItemName = "HealthPotion";
}

void AHealthPotion::Use(ACharacterBase* Player)
{
    if (ACharacterBase* UseChar = Cast<ACharacterBase>(Player))
    {
        UAbilitySystemComponent* ASC = UseChar->GetAbilitySystemComponent();
        if (ASC && HealEffect)
        {
            ASC->ApplyGameplayEffectToSelf(HealEffect->GetDefaultObject<UGameplayEffect>(), 1.0f, ASC->MakeEffectContext());
        }
    }
}