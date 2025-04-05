

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT8_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UMultiplayerSessionsSubsystem();


	TSharedPtr<FOnlineSessionSearch> SessionSearch;





	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	void QuickMatch();

	void OnFindSessionsComplete(bool bWasSuccessful);

	void CreateQuickMatchSession();

	IOnlineSessionPtr SessionInterface;

	//UFUNCTION(BlueprintCallable)
	//void CreateServer(FString ServerName);
	//UFUNCTION(BlueprintCallable)
	//void FindServer(FString ServerName);

	void OnCreateSessionComplete(FName SessionName, bool WasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnQuickMatchReady();
	void TravelToGameLevel();
	void CreateGameSession(FString ServerName);
	void JoinGameSession();
	void StartGameFromLobby();
	int32 GetCurrentPlayerCount();
	FString GetRandomMap();
	FString GetRandomMode();
	void KickPlayer(const FUniqueNetIdRepl& PlayerId);
};
