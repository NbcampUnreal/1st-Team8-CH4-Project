#include "Item/SpeedBoostPotion.h"

ASpeedBoostPotion::ASpeedBoostPotion()
{
	ItemName = "SpeedBoostPotion";
}

void ASpeedBoostPotion::Use(ACharacterBase* Player)
{
    if (Player)
    {
        Player->SpeedUpStart(SpeedIncreaseRatio);
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [Player]() {if (Player) { Player->SpeedUpEnd(); }}, Duration, false);
        Super::Use(Player);
    }
}