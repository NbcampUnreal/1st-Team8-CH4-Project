#include "Item/PoisonKnife.h"
#include "Player/CharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

APoisonKnife::APoisonKnife()
{
    ItemName = "PoisonKnife";
}

void APoisonKnife::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

    if (ACharacterBase* HitChar = Cast<ACharacterBase>(OtherActor))
    {
        if (UAbilitySystemComponent* ASC = HitChar->GetAbilitySystemComponent())
        {
            if (PoisonEffect)
            {
                ASC->ApplyGameplayEffectToSelf(PoisonEffect->GetDefaultObject<UGameplayEffect>(), 1.0f, ASC->MakeEffectContext());
            }
        }
    }
}