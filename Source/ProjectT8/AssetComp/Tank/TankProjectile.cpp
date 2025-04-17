


#include "AssetComp/Tank/TankProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Player/CharacterBase.h"
#include "Engine/OverlapResult.h"
// Sets default values
ATankProjectile::ATankProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileComp"));
	RootComponent = ProjectileComp;

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
}

// Called when the game starts or when spawned
void ATankProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileComp->OnComponentHit.AddDynamic(this, &ATankProjectile::OnHit);
}

// Called every frame
void ATankProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATankProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParams;
	bool HitResult = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		ProjectileComp->GetComponentLocation(),
		FQuat::Identity,
		ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(150.f),
		CollisionQueryParams
	);

	if (HitResult)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			ACharacterBase* Player = Cast<ACharacterBase>(OverlapResult.GetActor());
			if (Player)
			{
				FString Name = Player->GetName();
				UE_LOG(LogTemp, Warning, TEXT("Name %s"), *Name);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Null"));
			}
		}
	}

	DrawDebugSphere(GetWorld(), ProjectileComp->GetComponentLocation(), 150.f, 16, FColor::Green, false, 1.0f);

	Destroy();
}

