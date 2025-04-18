#include "Throwable.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Player/CharacterBase.h"
#include "Player/Component/ItemComponent.h"
#include "TimerManager.h"
#include "AbilitySystemComponent.h"

AThrowable::AThrowable()
{
    PrimaryActorTick.bCanEverTick = true;
    SetReplicates(true); 
    bIsThrown = false;

    ItemMesh->OnComponentHit.AddDynamic(this, &AThrowable::OnHit);

    EffectCollision = CreateDefaultSubobject<USphereComponent>(TEXT("EffectCollision"));
    EffectCollision->SetupAttachment(ItemMesh);
    EffectCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EffectCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    EffectCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    InteractSphere->SetSphereRadius(20.f);
    EffectCollision->SetSphereRadius(50.f);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->bAutoActivate = false;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->ProjectileGravityScale = 1.f;
}

void AThrowable::BeginPlay()
{
    Super::BeginPlay();
}

void AThrowable::Use(ACharacterBase* Player)
{
    if (!Player || bIsThrown) return;

    FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
    DetachFromActor(DetachRules);

    ItemMesh->SetSimulatePhysics(false);
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    FVector ThrowDirection = Player->GetActorForwardVector() + FVector(0, 0, 0.3f);
    ProjectileMovement->Velocity = ThrowDirection * ThrowForce;
    ProjectileMovement->Activate();

    bIsThrown = true;

    if (UItemComponent* ItemComp = Player->FindComponentByClass<UItemComponent>())
    {
        ItemComp->SetEquippedItem(nullptr);
    }
}

void AThrowable::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!bIsThrown || !HasAuthority()) return;

    EffectCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    EffectCollision->SetSphereRadius(EffectRadius);

    TArray<AActor*> OverlappingActors;
    EffectCollision->GetOverlappingActors(OverlappingActors, ACharacterBase::StaticClass());

    TSubclassOf<UGameplayEffect> EffectToApply = GetEffectToApply();
    if (!EffectToApply) return;

    for (AActor* Actor : OverlappingActors)
    {
        ACharacterBase* Char = static_cast<ACharacterBase*>(Actor);
        if (UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent())
        {
            ASC->ApplyGameplayEffectToSelf(
                EffectToApply->GetDefaultObject<UGameplayEffect>(),
                1.0f,
                ASC->MakeEffectContext()
            );
        }
    }

    Destroy();
}