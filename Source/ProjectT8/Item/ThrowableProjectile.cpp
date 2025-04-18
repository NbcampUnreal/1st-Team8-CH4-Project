#include "Item/ThrowableProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayEffect.h"
#include "Player/CharacterBase.h"
#include "AbilitySystemComponent.h"

AThrowableProjectile::AThrowableProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);

	ProjectileMesh->SetSimulatePhysics(false);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProjectileMesh->SetNotifyRigidBodyCollision(true);
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AThrowableProjectile::OnHit);
}

void AThrowableProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AThrowableProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority()) return;

	if (ACharacterBase* Char = Cast<ACharacterBase>(OtherActor))
	{
		if (UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent())
		{
			ASC->ApplyGameplayEffectToSelf(
				GetEffectToApply()->GetDefaultObject<UGameplayEffect>(),
				1.0f,
				ASC->MakeEffectContext()
			);
			UE_LOG(LogTemp, Warning, TEXT("[Effect] Applied to: %s"), *Char->GetName());
		}
	}

	Destroy();

	/*TSubclassOf<UGameplayEffect> EffectToApply = GetEffectToApply();
	if (!EffectToApply) return;

	UE_LOG(LogTemp, Warning, TEXT("```````ThrowableProjectile OnHit: %s`````````````"), *GetName());
	DrawDebugSphere(GetWorld(), GetActorLocation(), 30.f, 12, FColor::Red, false, 3.f);

	TArray<AActor*> OverlappingActors;
	ProjectileMesh->GetOverlappingActors(OverlappingActors, ACharacterBase::StaticClass());

	bool bAppliedEffect = false;
	for (AActor* Actor : OverlappingActors)
	{
		if (ACharacterBase* Char = Cast<ACharacterBase>(Actor))
		{
			if (UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent())
			{
				ASC->ApplyGameplayEffectToSelf(
					EffectToApply->GetDefaultObject<UGameplayEffect>(),
					1.0f,
					ASC->MakeEffectContext()
				);
				bAppliedEffect = true;
			}
		}
	}
	if (bAppliedEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Hit] Total overlaps: %d"), OverlappingActors.Num());
		Destroy();
	}*/

}

TSubclassOf<UGameplayEffect> AThrowableProjectile::GetEffectToApply() const
{
	return nullptr;
}