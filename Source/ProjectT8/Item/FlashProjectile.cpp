#include "Item/FlashProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

AFlashProjectile::AFlashProjectile()
{
	if (MovementComp)
	{
		MovementComp->InitialSpeed = 1000.f;;
		MovementComp->MaxSpeed = 1000.f;;
		MovementComp->bRotationFollowsVelocity = true;
		MovementComp->bShouldBounce = true;
		MovementComp->ProjectileGravityScale = 1.f;
	}
}