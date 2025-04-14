#include "Multi/Widget/UserSlot.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Common/T8PlayerState.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "Kismet/GameplayStatics.h"

void UUserSlot::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Set defaults
    bIsAI = false;
    bIsHost = false;
    SlotIndex = -1;
    PlayerStateRef = nullptr;
    
    // Setup UI
    if (CharacterRenderTargetToMaterial)
    {
        SetCharacterImage(CharacterRenderTargetToMaterial);
    }
    
    // Bind kick button click event
    if (KickButton)
    {
        KickButton->OnClicked.AddDynamic(this, &UUserSlot::OnKickButtonClicked);
        KickButton->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UUserSlot::SetupSlot(const FString& DisplayName, APlayerState* InPlayerState, bool bInIsAI, int32 InSlotIndex, bool bInIsHost)
{
    PlayerStateRef = InPlayerState;
    SlotIndex = InSlotIndex;
    bIsAI = bInIsAI;
    bIsHost = bInIsHost;
    
    // Set display name
    if (CharacterName)
    {
        CharacterName->SetText(FText::FromString(DisplayName));
    }
    
    // Select appropriate character image
    if (CharacterImage)
    {
        if (bIsAI && AICharacterMaterial)
        {
            SetCharacterImage(AICharacterMaterial);
        }
        else if (CharacterRenderTargetToMaterial)
        {
            SetCharacterImage(CharacterRenderTargetToMaterial);
        }
    }
    
    // Only show kick button if local player is host and this isn't the host's slot
    bool bCanKick = false;
    if (bIsHost)
    {
        APlayerController* LocalPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (LocalPC && LocalPC->PlayerState)
        {
            // Can kick if not your own slot
            bCanKick = (LocalPC->PlayerState != PlayerStateRef) || bIsAI;
        }
    }
    
    ShowKickButton(bCanKick);
}

void UUserSlot::SetCharacterName(const FText& NewText)
{
    if (CharacterName)
    {
        CharacterName->SetText(NewText);
    }
}

void UUserSlot::SetCharacterImage(UMaterialInterface* NewMaterial)
{
    if (CharacterImage && NewMaterial)
    {
        CharacterImage->SetBrushFromMaterial(NewMaterial);
    }
}

void UUserSlot::SetUserPersonaName()
{
    FString PersonaName = TEXT("Unknown User");
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (APlayerState* PS = PC->PlayerState)
        {
            if (AT8PlayerState* T8PS = Cast<AT8PlayerState>(PS))
            {
                PersonaName = T8PS->PersonaName;
            }
            else
            {
                // Fallback to standard player name
                PersonaName = PS->GetPlayerName();
            }
        }
    }
    if (CharacterName)
    {
        CharacterName->SetText(FText::FromString(PersonaName));
    }
}

void UUserSlot::ShowKickButton(bool bShow)
{
    if (KickButton)
    {
        KickButton->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UUserSlot::OnKickButtonClicked()
{
    if (SlotIndex < 0) return;
    
    UE_LOG(LogTemp, Log, TEXT("Kick button clicked for slot %d"), SlotIndex);
    
    // Only the host should be able to kick
    APlayerController* LocalPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!LocalPC || !LocalPC->HasAuthority()) return;
    
    // Get the LobbyGameState and remove player or AI from slot
    ALobbyGameState* LobbyState = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
    if (LobbyState)
    {
        if (bIsAI)
        {
            // Simply remove the AI from the slot
            LobbyState->RemoveSlotOccupant(SlotIndex);
            UE_LOG(LogTemp, Log, TEXT("AI removed from slot %d"), SlotIndex);
        }
        else if (PlayerStateRef)
        {
            // For real players, the host can kick them
            LobbyState->RemovePlayerFromSlot(PlayerStateRef);
            UE_LOG(LogTemp, Log, TEXT("Player kicked from slot %d"), SlotIndex);
        }
    }
}