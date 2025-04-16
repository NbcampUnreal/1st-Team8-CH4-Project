#include "T8GameInstance.h"
#include "GameFramework/GameState/LobbyGameState.h"

void UT8GameInstance::SaveLobbyGameState(ALobbyGameState* LobbyGameState)
{
    if (!LobbyGameState)
        return;

    // LobbyGameState의 정보 복사
    SavedLobbySlots = LobbyGameState->Slots;
    SavedTeamSetup = LobbyGameState->TeamSetup;
    SavedSelectedMap = LobbyGameState->SelectedMap;

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
