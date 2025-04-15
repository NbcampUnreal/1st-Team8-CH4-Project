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
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ItemMesh->SetCollisionResponseToAllChannels(ECR_Block);
    ItemMesh->SetCollisionObjectType(ECC_PhysicsBody);

    ItemMesh->SetSimulatePhysics(true);
    ItemMesh->SetEnableGravity(true);

    // 스케일 강제 고정 (테스트용)
    SetActorScale3D(FVector(2.0f));


    FVector ThrowDirection = Player->GetActorForwardVector() + FVector(0, 0, 0.5f);
    ItemMesh->AddImpulse(ThrowDirection * ThrowForce, NAME_None, true);

    bIsThrown = true;
    /*if (!Player || bIsThrown) return;

    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    FVector ThrowDirection = Player->GetActorForwardVector() + FVector(0, 0, 0.5f);
    ItemMesh->AddImpulse(ThrowDirection * ThrowForce, NAME_None, true);

    bIsThrown = true;*/
}

void AThrowable::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!bIsThrown) return;

    Destroy();
}