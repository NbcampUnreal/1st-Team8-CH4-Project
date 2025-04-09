#include "Throwable.h"
#include "Engine/World.h"
#include "Player/CharacterBase.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"

AThrowable::AThrowable()
{
    ItemMesh->SetSimulatePhysics(true);
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    ItemMesh->OnComponentHit.AddDynamic(this, &AThrowable::OnHit);
    PrimaryActorTick.bCanEverTick = true;
}

void AThrowable::BeginPlay()
{
    Super::BeginPlay();
}

void AThrowable::Use(ACharacterBase* Player)
{
    if (Player)
    {
        DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        FVector ThrowDirection = Player->GetActorForwardVector() + FVector(0, 0, 0.5f);
        ItemMesh->AddImpulse(ThrowDirection * ThrowForce);
        bIsThrown = true;
    }
}

void AThrowable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (bIsThrown && ThrowDuration > 0.0f)
    {
        ThrowDuration -= DeltaTime;
        if (ThrowDuration <= 0.0f)
        {
            Explode();
        }
    }
}

void AThrowable::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (ThrowDuration <= 0.0f) // 즉시 효과 적용
    {
        if (ACharacterBase* Target = Cast<ACharacterBase>(OtherActor))
        {
            ApplyEffect(Target);
        }
        Destroy();
    }
}

void AThrowable::Explode()
{
    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    if (GetWorld()->OverlapMultiByChannel(Overlaps, GetActorLocation(), FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(EffectRadius), Params))
    {
        for (const FOverlapResult& Result : Overlaps)
        {
            if (ACharacterBase* Target = Cast<ACharacterBase>(Result.GetActor()))
            {
                ApplyEffect(Target);
            }
        }
    }
    Destroy();
}

void AThrowable::ApplyEffect(ACharacterBase* Target)
{
    // 기본 효과 없음, 하위 클래스에서 오버라이드
}