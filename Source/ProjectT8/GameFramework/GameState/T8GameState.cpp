#include "GameFramework/GameState/T8GameState.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "GameFramework/Common/T8GameInstance.h"
#include "UI/TeamStatusBar.h"
#include "Net/UnrealNetwork.h"

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
    return 0;
}

void AT8GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AT8GameState, PlayerUIList);
}

void AT8GameState::InitializePlayerUIList(const TArray<FSlotInfo>& SlotList)
{
    PlayerUIList.Empty();

    for (const FSlotInfo& Slot : SlotList)
    {
        FPlayerStatusUIData UIData;

        UIData.DisplayName = Slot.DisplayName;
        UIData.bIsAlive = true;
        UIData.bIsAI = Slot.bIsAI;

        switch (Slot.TeamNumber)
        {
        case 0: UIData.TeamColor = FLinearColor::Red; break;
        case 1: UIData.TeamColor = FLinearColor::Blue; break;
        case 2: UIData.TeamColor = FLinearColor::Green; break;
        case 3: UIData.TeamColor = FLinearColor::Yellow; break;
        case 4: UIData.TeamColor = FLinearColor(1.f, 0.5f, 0.f); break;   // 주황색 (Orange)
        case 5: UIData.TeamColor = FLinearColor(0.5f, 0.f, 1.f); break;   // 보라색 (Purple)
        case 6: UIData.TeamColor = FLinearColor(0.f, 1.f, 1.f); break;    // 시안 (Cyan)
        case 7: UIData.TeamColor = FLinearColor(1.f, 0.f, 1.f); break;    // 마젠타 (Magenta)
        default: UIData.TeamColor = FLinearColor::Gray; break;
        }

        PlayerUIList.Add(UIData);
    }
}
