


#include "AssetComp/Tank/Tank.h"
#include "Player/CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "TankProjectile.h"
#include "DrawDebugHelpers.h"

ATank::ATank()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	TankMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TankMesh"));
	TankMesh->SetupAttachment(RootComp);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(TankMesh);

	CannonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CannonMesh"));
	CannonMesh->SetupAttachment(TurretMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(CannonMesh);

	DefalutRot = FRotator::ZeroRotator;

}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterBase::StaticClass(), FoundActors);
	
	if (FoundActors.Num() > 0)
	{
		PlayerCharacter = Cast<ACharacterBase>(FoundActors[0]);
	}
	//Delegate.BindUFunction(this, "InvertBool", bCanAiming);
	//GetWorldTimerManager().SetTimer(TurnTimerHandle, Delegate, TurnRate, true);
	GetWorldTimerManager().SetTimer(TurnTimerHandle, this, &ATank::Aiming, TurnRate, true);
}

void ATank::TurnTurret(FVector LookAtTartget)
{
	float DeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(this);
	if (PlayerCharacter)
	{
		if (bCanAiming)
		{
			FVector ToTarget = LookAtTartget - TurretMesh->GetComponentLocation();
			
			FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw - 90.f, 0.f); // Yaw -90.f
			TurretMesh->SetWorldRotation(FMath::RInterpTo(TurretMesh->GetComponentRotation(),LookAtRotation, DeltaSeconds,5.f));

			FRotator LookAtCannonRotation = FRotator(0.f, ToTarget.Rotation().Yaw -90.f, ToTarget.Rotation().Pitch * -1); // Yaw -90.f Pitch * -1
			CannonMesh->SetWorldRotation(FMath::RInterpTo(CannonMesh->GetComponentRotation(), LookAtCannonRotation, DeltaSeconds, 5.f));

			Distance = FVector::Distance(PlayerCharacter->GetActorLocation(), RootComp->GetComponentLocation());
			RollValue;
			if (Distance > 1500)
			{
				RollValue = 7.f;
			}
			else if (Distance <= 1500)
			{
				RollValue = 15.f;
			}
			FRotator LookAtSpawnRot = FRotator(ToTarget.Rotation().Roll - RollValue, ToTarget.Rotation().Yaw, ((ToTarget.Rotation().Pitch * -1) -93.f)); //  ((Pitch * -1) -90.f)
			ProjectileSpawnPoint->SetWorldRotation(FMath::RInterpTo(ProjectileSpawnPoint->GetComponentRotation(), LookAtSpawnRot, DeltaSeconds, 5.f));
		}
		else
		{
			TurretMesh->SetWorldRotation(FMath::RInterpTo(TurretMesh->GetComponentRotation(), DefalutRot, DeltaSeconds, 5.f));
			CannonMesh->SetWorldRotation(FMath::RInterpTo(CannonMesh->GetComponentRotation(), DefalutRot, DeltaSeconds, 5.f));
			ProjectileSpawnPoint->SetWorldRotation(FMath::RInterpTo(CannonMesh->GetComponentRotation(), DefalutRot, DeltaSeconds, 5.f));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter is null"));
	}
}

// Called when the game starts or when spawned
void ATank::Fire()
{
	if (ProjectileClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectileClass is null"));
		return;
	}
	FVector Lot = ProjectileSpawnPoint->GetComponentLocation();
	FRotator Rot = ProjectileSpawnPoint->GetComponentRotation();
	GetWorld()->SpawnActor<ATankProjectile>(ProjectileClass, Lot, Rot);
}

// Aiming
void ATank::Aiming()
{
	if (PlayerCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter is null"));
		return;
	}
	if (bCanAiming)
	{
		bCanAiming = false;
	}
	else
	{
		bCanAiming = true;
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATank::Fire, FireRate, true);
	}
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TurnTurret(PlayerCharacter->GetActorLocation());
}

