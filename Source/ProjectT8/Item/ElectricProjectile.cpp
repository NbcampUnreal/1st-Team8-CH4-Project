#include "Item/ElectricProjectile.h"
#include "Player/CharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameFramework/ProjectileMovementComponent.h"

AElectricProjectile::AElectricProjectile()
{
}

void AElectricProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

    if (ACharacterBase* HitChar = Cast<ACharacterBase>(OtherActor))
    {
        UAbilitySystemComponent* ASC = HitChar->GetAbilitySystemComponent();
        if (ASC && ShockEffect)
        {
            ASC->ApplyGameplayEffectToSelf(ShockEffect->GetDefaultObject<UGameplayEffect>(), 1.0f, ASC->MakeEffectContext());
        }
    }
}