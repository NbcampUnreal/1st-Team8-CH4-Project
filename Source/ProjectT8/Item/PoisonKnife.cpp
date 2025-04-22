#include "Item/PoisonKnife.h"
#include "Item/PoisonProjectile.h"

APoisonKnife::APoisonKnife()
{
    ItemName = "PoisonKnife";

	static ConstructorHelpers::FClassFinder<AProjectile> ProjectileBPClass(TEXT("/Game/Blueprints/Item/BP_PoisonProjectile"));
	if (ProjectileBPClass.Succeeded())
	{
		ProjectileClass = ProjectileBPClass.Class;
	}
}