#include "Item/Projectile.h"
#include "Components/SphereComponent.h"
#include "Player/CharacterBase.h"

AProjectile::AProjectile()
{
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    RootComponent = ProjectileMesh;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->SetupAttachment(RootComponent);
    CollisionSphere->SetSphereRadius(100.0f); // 충돌 범위 Test후 수정 必
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnSphereOverlap);

    PrimaryActorTick.bCanEverTick = true;
    SetLifeSpan(LifeSpan);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();	
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    FVector NewLocation = GetActorLocation() + GetActorForwardVector() * MoveSpeed * DeltaTime;
	SetActorLocation(NewLocation); // 직선이동말고 살짝 곡선 + 땅에 떨어지게 수정 必
}

void AProjectile::ApplyEffect(ACharacterBase* Target)
{
    // 각 총 종류별 효과 , 자식 클래스에서 오버라이드
}
void AProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ACharacterBase* Target = Cast<ACharacterBase>(OtherActor))
    {
        ApplyEffect(Target);
    }
}