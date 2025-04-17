#include "T8GameInstance.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameState/LobbyGameState.h"

void UT8GameInstance::SaveLobbyGameState(ALobbyGameState* LobbyGameState)
{
    if (!LobbyGameState)
        return;

    // LobbyGameState�� ���� ����
    SavedLobbySlots = LobbyGameState->Slots;
    SavedTeamSetup = LobbyGameState->TeamSetup;

    for (int32 i = 0; i < SavedLobbySlots.Num(); ++i)
    {
        const FSlotInfo& Slot = SavedLobbySlots[i];
        if (Slot.bIsAI)
        {
            UE_LOG(LogTemp, Log, TEXT("Saved AI slot %d: Name=%s, TeamNumber=%d"), 
                i, *Slot.DisplayName, Slot.TeamNumber);
        }
        else if (Slot.PlayerState)
        {
            UE_LOG(LogTemp, Log, TEXT("Saved Player slot %d: Name=%s, TeamNumber=%d"), 
                i, *Slot.DisplayName, Slot.TeamNumber);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Saved LobbyGameState to GameInstance: %d slots, TeamSetup: %d"),
        SavedLobbySlots.Num(), static_cast<int32>(SavedTeamSetup));
}

int32 UT8GameInstance::GetAICountForTeam(int32 TeamNumber) const
{
    int32 Count = 0;
    for (const FSlotInfo& Slot : SavedLobbySlots)
    {
        if (Slot.bIsAI && Slot.TeamNumber == TeamNumber)
        {
            Count++;
        }
    }
    return Count;
}

void UT8GameInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UT8GameInstance, WinningPlayerStatesResult);
}