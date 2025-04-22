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

}

TSubclassOf<UGameplayEffect> AThrowableProjectile::GetEffectToApply() const
{
	return nullptr;
}