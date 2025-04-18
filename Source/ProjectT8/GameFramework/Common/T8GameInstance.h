#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"

#include "GameFramework/GameState/LobbyGameState.h"

#include "T8GameInstance.generated.h"

class UPhaseInfoDataAsset;

UCLASS()
class PROJECTT8_API UT8GameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
    UPhaseInfoDataAsset* PhaseInfoDataAsset;

    UPROPERTY(BlueprintReadWrite, Category = "Game Data")
    TArray<FSlotInfo> SavedLobbySlots;

    UPROPERTY(BlueprintReadWrite, Category = "Game Data")
    ETeamSetup SavedTeamSetup;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game Result")
    TArray<APlayerState*> WinningPlayerStatesResult;

    // LobbyGameState�� ������ GameInstance�� ����
    UFUNCTION(BlueprintCallable, Category = "Game Data")
    void SaveLobbyGameState(ALobbyGameState* LobbyGameState);

    // BattleLevel�� GameState �ʱ�ȭ �� ���
    UFUNCTION(BlueprintCallable, Category = "Game Data")
    int32 GetAICountForTeam(int32 TeamNumber) const;

    const TArray<FSlotInfo>& GetSavedLobbySlots() const { return SavedLobbySlots; }

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};