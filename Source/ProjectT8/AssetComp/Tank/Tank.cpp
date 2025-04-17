


#include "AssetComp/Tank/Tank.h"
#include "Player/CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "TankProjectile.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

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

	SetReplicates(true);
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerCharacter = Cast<ACharacterBase>(UGameplayStatics::GetPlayerPawn(this, 0));
	GetWorldTimerManager().SetTimer(TurnTimerHandle, this, &ATank::CanAiming, TurnRate, true);
}


void ATank::OnRep_TurnTurret()
{
	float DeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(this);
	if (bCanAiming)
	{
		FVector ToTarget = PlayerCharacter->GetActorLocation() - TurretMesh->GetComponentLocation();
			
		FRotator LookAtTurretRotation = FRotator(0.f, ToTarget.Rotation().Yaw - 90.f, 0.f); // Yaw -90.f
		TurretMesh->SetWorldRotation(FMath::RInterpTo(TurretMesh->GetComponentRotation(), LookAtTurretRotation, DeltaSeconds,5.f));

		FRotator LookAtCannonRotation = FRotator(0.f, ToTarget.Rotation().Yaw -90.f, ToTarget.Rotation().Pitch * -1); // Yaw -90.f Pitch * -1
		CannonMesh->SetWorldRotation(FMath::RInterpTo(CannonMesh->GetComponentRotation(), LookAtCannonRotation, DeltaSeconds, 5.f));

		Distance = FVector::Distance(PlayerCharacter->GetActorLocation(), RootComp->GetComponentLocation());

		FRotator LookAtSpawnRot = FRotator(ToTarget.Rotation().Roll - DistanceToValue(Distance), ToTarget.Rotation().Yaw, ((ToTarget.Rotation().Pitch * -1) - 93.f)); //  ((Pitch * -1) -90.f)
		ProjectileSpawnPoint->SetWorldRotation(FMath::RInterpTo(ProjectileSpawnPoint->GetComponentRotation(), LookAtSpawnRot, DeltaSeconds, 5.f));
	}
	else
	{
		TurretMesh->SetWorldRotation(FMath::RInterpTo(TurretMesh->GetComponentRotation(), DefalutRot, DeltaSeconds, 5.f));
		CannonMesh->SetWorldRotation(FMath::RInterpTo(CannonMesh->GetComponentRotation(), DefalutRot, DeltaSeconds, 5.f));
		ProjectileSpawnPoint->SetWorldRotation(FMath::RInterpTo(CannonMesh->GetComponentRotation(), DefalutRot, DeltaSeconds, 5.f));
	}
}

bool ATank::Server_TurnTurret_Validate(ACharacterBase* Player)
{
	return true;
}

void ATank::Server_TurnTurret_Implementation(ACharacterBase* Player)
{
	PlayerCharacter = Player;
	OnRep_TurnTurret();
}
void ATank::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATank, PlayerCharacter);
}

bool ATank::Server_Fire_Validate()
{
	return true;
}

void ATank::Server_Fire_Implementation()
{
	Fire();
}

float ATank::DistanceToValue(float Value)
{
	if (Value > 1500)
	{
		return 7.f;
	}
	else if (Value <= 1500)
	{
		return 15.f;
	}
	else return 0.f;
}

// Called when the game starts or when spawned
void ATank::Fire()
{
	if (ProjectileClass == nullptr) return;
	if (HasAuthority())
	{
		if (bCanAiming)
		{
			FVector Lot = ProjectileSpawnPoint->GetComponentLocation();
			FRotator Rot = ProjectileSpawnPoint->GetComponentRotation();
			GetWorld()->SpawnActor<ATankProjectile>(ProjectileClass, Lot, Rot);
		}
	}
	else
	{
		Server_Fire();
	}
}

// Aiming
void ATank::CanAiming()
{
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
	OnRep_TurnTurret();
}