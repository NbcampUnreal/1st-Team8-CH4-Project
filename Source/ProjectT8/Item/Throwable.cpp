#include "Throwable.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Player/CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AThrowable::AThrowable()
{
    PrimaryActorTick.bCanEverTick = true;
    SetReplicates(true);

    ItemMesh->SetSimulatePhysics(true);
    ItemMesh->SetEnableGravity(true);
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ItemMesh->SetCollisionResponseToAllChannels(ECR_Block);
    ItemMesh->OnComponentHit.AddDynamic(this, &AThrowable::OnHit);
}

void AThrowable::BeginPlay()
{
    Super::BeginPlay();
}

void AThrowable::Use(ACharacterBase* Player)
{
    if (!Player || bIsThrown) return;

    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    FVector ThrowDirection = Player->GetActorForwardVector() + FVector(0, 0, 0.5f);
    ItemMesh->AddImpulse(ThrowDirection * ThrowForce, NAME_None, true);

    bIsThrown = true;
}

void AThrowable::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!bIsThrown) return;

    Destroy();
}