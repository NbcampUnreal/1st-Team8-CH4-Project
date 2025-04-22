#include "Item/Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Player/CharacterBase.h"

AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = false;
    UE_LOG(LogTemp,Warning,TEXT("Set Replicate func, AProjectile()"));
    SetReplicates(true);
    SetReplicateMovement(true);

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(10.0f);
    CollisionComp->SetCollisionProfileName("BlockAllDynamic");
    CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
    RootComponent = CollisionComp;

    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetupAttachment(RootComponent);
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    MovementComp->UpdatedComponent = CollisionComp;
    MovementComp->InitialSpeed = Speed;
    MovementComp->MaxSpeed = Speed;
    MovementComp->bRotationFollowsVelocity = true;
    MovementComp->bShouldBounce = false;
    MovementComp->ProjectileGravityScale = 0.f;

}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
    SetLifeSpan(LifeSpan);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != GetOwner())
    {
        Destroy();
    }
}
