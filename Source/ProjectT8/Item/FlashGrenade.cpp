#include "Item/FlashGrenade.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

AFlashGrenade::AFlashGrenade()
{
	ItemName = "FlashGrenade";
}

void AFlashGrenade::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

    TArray<AActor*> OverlappedActors;
    FVector Origin = GetActorLocation();

    // �ֺ� �÷��̾�
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterBase::StaticClass(), OverlappedActors);

    for (AActor* Actor : OverlappedActors)
    {
        if (ACharacterBase* Char = Cast<ACharacterBase>(Actor))
        {
            float Dist = FVector::Dist(Char->GetActorLocation(), Origin);
            if (Dist <= FlashRadius)
            {
                if (UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent())
                {
                    if (BlindEffect)
                    {
                        ASC->ApplyGameplayEffectToSelf(
                            BlindEffect->GetDefaultObject<UGameplayEffect>(),
                            1.0f,
                            ASC->MakeEffectContext()
                        );
                    }
                }
            }
        }
    }

    Destroy();
}