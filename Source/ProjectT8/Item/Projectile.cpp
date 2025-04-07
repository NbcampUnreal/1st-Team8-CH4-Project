#include "Item/Projectile.h"
#include "Components/SphereComponent.h"
#include "Player/CharacterBase.h"

AProjectile::AProjectile()
{
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    RootComponent = ProjectileMesh;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->SetupAttachment(RootComponent);
    CollisionSphere->SetSphereRadius(100.0f); // �浹 ���� Test�� ���� ��
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
	SetActorLocation(NewLocation); // �����̵����� ��¦ � + ���� �������� ���� ��
}

void AProjectile::ApplyEffect(ACharacterBase* Target)
{
    // �� �� ������ ȿ�� , �ڽ� Ŭ�������� �������̵�
}
void AProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ACharacterBase* Target = Cast<ACharacterBase>(OtherActor))
    {
        ApplyEffect(Target);
    }
}