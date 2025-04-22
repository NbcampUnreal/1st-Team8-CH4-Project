#include "Item/FlashGrenade.h"
#include "Item/FlashProjectile.h"

AFlashGrenade::AFlashGrenade()
{
	ItemName = "FlashGrenade";

	static ConstructorHelpers::FClassFinder<AProjectile> ProjectileBPClass(TEXT("/Game/Blueprints/Item/BP_FlashProjectile"));
	if (ProjectileBPClass.Succeeded())
	{
		ProjectileClass = ProjectileBPClass.Class;
	}
}