#include "Multi/Widget/UserSlot.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Common/T8PlayerState.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"

void UUserSlot::NativeConstruct()
{
    Super::NativeConstruct();
}

void UUserSlot::SetupSlot(const FString& DisplayName, APlayerState* InPlayerState, bool bInIsAI, int32 InSlotIndex, bool bInIsHost)
{
    PlayerStateRef = InPlayerState;
    bIsAI = bInIsAI;
    SlotIndex = InSlotIndex;
    bIsHost = bInIsHost;

    SetCharacterName(FText::FromString(DisplayName));
    
    if (bIsAI)
    {
        SetCharacterImage(AICharacterMaterial);
    }
    else
    {
        SetCharacterImage(CharacterRenderTargetToMaterial);
    }
}

void UUserSlot::SetCharacterName(const FText& NewText)
{
    if (CharacterName)
    {
        CharacterName->SetText(NewText);
    }
}

void UUserSlot::SetUserPersonaName()
{
    if (PlayerStateRef)
    {
        SetCharacterName(FText::FromString(PlayerStateRef->GetPlayerName()));
    }
}

void UUserSlot::SetCharacterImage(UMaterialInterface* NewMaterial)
{
    if (CharacterImage)
    {
        CharacterImage->SetBrushFromMaterial(NewMaterial);
    }
}