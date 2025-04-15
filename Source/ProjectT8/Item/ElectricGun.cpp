#include "Item/ElectricGun.h"
#include "Item/ElectricProjectile.h"
AElectricGun::AElectricGun()
{
    ItemName = "ElectricGun";
    static ConstructorHelpers::FClassFinder<AProjectile> ProjectileBPClass(TEXT("/Game/Blueprints/Item/BP_ElectricProjectile"));
    if (ProjectileBPClass.Succeeded())
    {
        ProjectileClass = ProjectileBPClass.Class;
    }    
}