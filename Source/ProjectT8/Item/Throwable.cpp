#include "Throwable.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Player/CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Player/Component/ItemComponent.h"

AThrowable::AThrowable()
{
    DrawDebugSphere(GetWorld(), GetActorLocation(), 50.f, 12, FColor::Red, false, 1.f);
    PrimaryActorTick.bCanEverTick = true;
    SetReplicates(true);

    ItemMesh->OnComponentHit.AddDynamic(this, &AThrowable::OnHit);
}

void AThrowable::BeginPlay()
{
    Super::BeginPlay();
}

void AThrowable::Use(ACharacterBase* Player)
{
    if (!Player || bIsThrown) return;

    // 스케일 확인용 로그
    UE_LOG(LogTemp, Warning, TEXT("Before Detach - Scale: %s"), *GetActorScale3D().ToString());

    FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
    DetachFromActor(DetachRules);

    // 확실하게 물리 재설정
    ItemMesh->SetSimulatePhysics(false);

    ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ItemMesh->SetCollisionResponseToAllChannels(ECR_Block);
    ItemMesh->SetCollisionObjectType(ECC_PhysicsBody);

    ItemMesh->SetSimulatePhysics(true);
    ItemMesh->SetEnableGravity(true);

    FVector ThrowDirection = Player->GetActorForwardVector() + FVector(0, 0, -0.1f);
    ItemMesh->AddImpulse(ThrowDirection * ThrowForce, NAME_None, true);

    bIsThrown = true;

    if (UItemComponent* ItemComp = Player->FindComponentByClass<UItemComponent>())
    {
        ItemComp->SetEquippedItem(nullptr);
    }
    
    /*if (!Player || bIsThrown) return;

    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    FVector ThrowDirection = Player->GetActorForwardVector() + FVector(0, 0, 0.5f);
    ItemMesh->AddImpulse(ThrowDirection * ThrowForce, NAME_None, true);

    bIsThrown = true;*/

}

void AThrowable::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!bIsThrown) return;
    UE_LOG(LogTemp, Warning, TEXT("[Throwable] OnHit: %s hit %s"), *GetNameSafe(this), *GetNameSafe(OtherActor));
    Destroy();
}