#if 0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Engine/NetSerialization.h"
#include "LobbyDataTypes.generated.h"

UENUM(BlueprintType)
enum class ELobbyTeamMode : uint8
{
    FreeForAll UMETA(DisplayName = "Free For All"),
    TwoTeams   UMETA(DisplayName = "Two Teams"),
    FourTeams  UMETA(DisplayName = "Four Teams")
};

USTRUCT(BlueprintType)
struct FLobbySlotInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Lobby Slot")
    FUniqueNetIdRepl PlayerId; // Unique ID of the player in this slot

    UPROPERTY(BlueprintReadOnly, Category = "Lobby Slot")
    bool bIsAI = false; // Is this slot occupied by an AI?

    UPROPERTY(BlueprintReadOnly, Category = "Lobby Slot")
    FString DisplayName = TEXT("Empty Slot"); // Player name or Bot name

    UPROPERTY(BlueprintReadOnly, Category = "Lobby Slot")
    int32 TeamId = -1; // -1 for unassigned, 0 for Team A, 1 for Team B etc.

    UPROPERTY(BlueprintReadOnly, Category = "Lobby Slot")
    bool bIsReady = false; // Player ready status

    UPROPERTY(BlueprintReadOnly, Category = "Lobby Slot")
    bool bIsHost = false; // Is this player the host?

    // Add other relevant info like avatar index, etc.

    bool IsOccupied() const {
        return PlayerId.IsValid() || bIsAI;
    }
};

USTRUCT(BlueprintType)
struct FGameSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Game Settings")
    FString SelectedMapName = TEXT("DefaultMap");

    UPROPERTY(BlueprintReadOnly, Category = "Game Settings")
    ELobbyTeamMode SelectedTeamMode = ELobbyTeamMode::FreeForAll;

    // Add other settings like max players, game duration etc.
};

#endif
