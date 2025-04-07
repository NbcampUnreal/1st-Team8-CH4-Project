#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FGameModeData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Mode")
    FString ModeName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Mode")
    int32 RequiredPlayers;

    FGameModeData()
        : ModeName(TEXT("")), RequiredPlayers(0)
    {
    }

    FGameModeData(FString InModeName, int32 InRequiredPlayers)
        : ModeName(InModeName), RequiredPlayers(InRequiredPlayers)
    {
    }
};

UCLASS()
class PROJECTT8_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UMultiplayerSessionsSubsystem();
    // 오버라이드 함수
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Quick Match 관련 기능
    UFUNCTION(BlueprintCallable, Category = "Multiplayer Sessions")
    void QuickMatch();

    void OnFindQuickSessionsComplete(bool bWasSuccessful);
    void CreateQuickMatchSession();
    void OnCreateQuickSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnJoinQuickSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnQuickMatchReady();
    void TravelToGameLevel();

    void CreateGameSession(FString ServerName);

    void JoinGameSession();

    void StartGameFromLobby();

    // 유틸리티 함수들
    FGameModeData GetRandomMode();
    void KickPlayer(const FUniqueNetIdRepl& PlayerId);
    FString GetRandomMap();
    int32 GetCurrentPlayerCount();

private:
    // 세션 인터페이스 관련 변수
    IOnlineSessionPtr SessionInterface;
    TSharedPtr<class FOnlineSessionSearch> SessionSearch;

    // Quick Match 세션 생성 시 선택한 랜덤 맵 경로 (나중에 레벨 이동 시 사용)
    FString QuickMatchSelectedMap;
    int32 QuickMatchRequiredPlayers;
    FTimerHandle QuickMatchPollTimerHandle;

    void PollQuickMatchPlayerCount();
};
