#include "Item/PoisonProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

APoisonProjectile::APoisonProjectile()
{
	if (MovementComp)
	{
		MovementComp->InitialSpeed = 1000.f;
		MovementComp->MaxSpeed = 1000.f;
		MovementComp->ProjectileGravityScale = 1.f;
		MovementComp->bShouldBounce = true;
		MovementComp->bRotationFollowsVelocity = true;
		MovementComp->bAutoActivate = true;
	}
}