#include "Item/Gun.h"
#include "Projectile.h"

AGun::AGun()
{
    WeaponType = EWeaponType::Gun;
    ItemName = "Gun";
}

void AGun::Attack()
{
    if (CurrentAmmo > 0 && ProjectileClass)
    {
        FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f; // �ѱ� ��ġ ���� ��
        FRotator SpawnRotation = GetActorRotation();
        GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);

        CurrentAmmo--;
        if (CurrentAmmo <= 0) Destroy();
    }
}