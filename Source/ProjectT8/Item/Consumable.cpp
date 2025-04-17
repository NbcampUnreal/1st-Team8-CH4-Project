#include "Item/Consumable.h"
#include "Player/Component/ItemComponent.h"

AConsumable::AConsumable()
{
    ItemType = EItemType::Consumable;
}

void AConsumable::Use(ACharacterBase* Player)
{
    if (!Player) return;

    if (UItemComponent* ItemComp = Player->FindComponentByClass<UItemComponent>())
    {
        ItemComp->SetEquippedItem(nullptr);
    }

    Destroy();
}