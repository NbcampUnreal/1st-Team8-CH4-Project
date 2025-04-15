#include "Item/Gun.h"
#include "Projectile.h"
#include "Net/UnrealNetwork.h"
#include "Player/CharacterBase.h"

AGun::AGun()
{
    WeaponType = EWeaponType::Gun;
    ItemName = "Gun";
    PrimaryActorTick.bCanEverTick = false;
    SetReplicates(true);
    CurrentAmmo = MaxAmmo;
}

void AGun::Use(ACharacterBase* Player)
{
    if (!Player) return;

    if (!HasAuthority())
    {
        Server_Use(Player);
        return;
    }
    if (CurrentAmmo > 0 && ProjectileClass)
    {
        FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f; // ÃÑ±¸ À§Ä¡ Á¶Á¤ ù±
        FRotator SpawnRotation = GetActorRotation();

        FActorSpawnParameters Params;
        Params.Owner = this;
        Params.Instigator = Player;

        GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, Params);

        --CurrentAmmo;
        if (CurrentAmmo <= 0) Destroy();
    }
}

void AGun::Server_Use_Implementation(ACharacterBase* Player)
{
    Use(Player);
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGun, CurrentAmmo);
}