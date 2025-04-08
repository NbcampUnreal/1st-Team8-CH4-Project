#include "Item/ElectricGun.h"
#include "Item/ElectricProjectile.h"
AElectricGun::AElectricGun()
{
    ItemName = "ElectricGun";
    ProjectileClass = AElectricProjectile::StaticClass();
}