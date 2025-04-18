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
	if (!Player || !ProjectileClass) return;

	APlayerController* PC = Cast<APlayerController>(Player->GetController());
	if (!PC) return;
	FVector FacingDirection = Player->GetActorForwardVector(); // Z축 회전 기준
	FVector SpawnLocation = GetActorLocation() + FacingDirection * 100.0f;
	FRotator SpawnRotation = FacingDirection.Rotation();

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);

	Destroy();
}