#include "GameFramework/GameState/T8GameState.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "GameFramework/Common/T8GameInstance.h"

void AT8GameState::BeginPlay()
{
    Super::BeginPlay();

    SetGamePhase(EGamePhase::Playing);

    if (HasAuthority())
    {
        UT8GameInstance* GameInstance = Cast<UT8GameInstance>(GetGameInstance());
        if (GameInstance)
        {
			const TArray<FSlotInfo>& SavedSlots = GameInstance->GetSavedLobbySlots();
			for (const FSlotInfo& Slot : SavedSlots)
			{
				if (Slot.bIsAI || Slot.PlayerState != nullptr)
				{
                    int32 TeamID = Slot.TeamNumber;
                    if (!ActiveTeamCounts.Contains(TeamID))
                    {
						ActiveTeamCounts.Add(TeamID, 0);
                    }
                    ActiveTeamCounts[TeamID]++;
				}
			}
        }
    }
}

void AT8GameState::RemovePlayerFromTeam(int32 TeamID)
{
	if (!HasAuthority()) return;
    if (ActiveTeamCounts.Contains(TeamID))
    {
        ActiveTeamCounts[TeamID]--;
        if (ActiveTeamCounts[TeamID] <= 0)
        {
            ActiveTeamCounts.Remove(TeamID);
        }
    }
}

bool AT8GameState::IsOnlyOneTeamRemaining(int32& OutLastTeamID)
{
    if (ActiveTeamCounts.Num() == 1)
    {
        for (const TPair<int32, int32>& Pair : ActiveTeamCounts)
        {
            OutLastTeamID = Pair.Key;
			return true;
        }
    }
    return false;
}

int32 AT8GameState::GetActiveTeamCount() const
{
    if (ActiveTeamCounts.Num() > 0)
    {
        return ActiveTeamCounts.Num();
    }
}
