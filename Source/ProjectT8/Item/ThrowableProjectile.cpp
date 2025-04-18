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

	ProjectileMesh->SetSimulatePhysics(true);
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


	TSubclassOf<UGameplayEffect> EffectToApply = GetEffectToApply();
	if (!EffectToApply) return;

	TArray<AActor*> OverlappingActors;
	ProjectileMesh->GetOverlappingActors(OverlappingActors, ACharacterBase::StaticClass());

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
			}
		}
	}

	Destroy();
}

TSubclassOf<UGameplayEffect> AThrowableProjectile::GetEffectToApply() const
{
	return nullptr;
}