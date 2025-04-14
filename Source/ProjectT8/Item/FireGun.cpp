#include "Item/FireGun.h"
#include "FireProjectile.h"

AFireGun::AFireGun()
{
    ItemName = "FireGun";
    static ConstructorHelpers::FClassFinder<AProjectile> ProjectileBPClass(TEXT("/Game/Blueprints/Item/BP_FireProjectile"));
    if (ProjectileBPClass.Succeeded())
    {
        ProjectileClass = ProjectileBPClass.Class;
    }
}