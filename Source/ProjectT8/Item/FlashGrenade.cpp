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
    UE_LOG(LogTemp, Warning, TEXT("[FlashGrenade] bIsThrown: %d"), bIsThrown);
    if (!bIsThrown) return;

    Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
    UE_LOG(LogTemp, Warning, TEXT("[FlashGrenade] OnHit Called"));

    TArray<AActor*> OverlappedActors;
    FVector Origin = GetActorLocation();
    UE_LOG(LogTemp, Warning, TEXT("[FlashGrenade] HitActor: %s"), *OtherActor->GetName());
    // 주변 플레이어
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