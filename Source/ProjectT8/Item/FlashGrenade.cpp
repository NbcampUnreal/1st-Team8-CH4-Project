#include "Item/FlashGrenade.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

AFlashGrenade::AFlashGrenade()
{
	ItemName = "FlashGrenade";
    EffectRadius = BlindRadius;
	ItemMesh->SetWorldScale3D(FVector(3.5f));
	InteractSphere->SetSphereRadius(15.0f);
}
