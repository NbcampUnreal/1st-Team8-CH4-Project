#include "Item/PoisonProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

APoisonProjectile::APoisonProjectile()
{
	if (MovementComp)
	{
		MovementComp->InitialSpeed = Speed;
		MovementComp->MaxSpeed = Speed;
		MovementComp->bRotationFollowsVelocity = true;
		MovementComp->bShouldBounce = true;            
		MovementComp->ProjectileGravityScale = 1.f;
	}
}