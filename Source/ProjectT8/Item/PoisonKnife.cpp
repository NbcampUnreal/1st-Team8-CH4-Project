#include "Item/PoisonKnife.h"
#include "Player/CharacterBase.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

APoisonKnife::APoisonKnife()
{
    ItemName = "PoisonKnife";
    EffectRadius = PoisonRadius;
}