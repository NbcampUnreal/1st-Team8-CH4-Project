#include "Item/Throwable.h"
#include "Item/Projectile.h"
#include "Player/CharacterBase.h"
#include "Kismet/GameplayStatics.h"

AThrowable::AThrowable()
{
	ItemName = TEXT("Throwable");
}

void AThrowable::Use(ACharacterBase* Player)
{
	if (!Player) return;

	if (!HasAuthority()) {
		Server_Use(Player);
		return;
	}

	if (ProjectileClass)
	{
	APlayerController* PC = Cast<APlayerController>(Player->GetController());
	if (!PC) return;
	FVector FacingDirection = Player->GetActorForwardVector(); // Z축 회전 기준
	FVector SpawnLocation = GetActorLocation() + FacingDirection * 100.0f;
	FRotator SpawnRotation = FacingDirection.Rotation();

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
	if (Projectile)
	{
	UE_LOG(LogTemp, Warning, TEXT("[8888888 Projectile SpawnWorld 88888888]"));
	}
	Destroy();
	}
}