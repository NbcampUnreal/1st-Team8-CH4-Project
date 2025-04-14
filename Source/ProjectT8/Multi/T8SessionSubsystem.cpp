#include "Multi/T8SessionSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "OnlineSubsystemUtils.h"


UT8SessionSubsystem::UT8SessionSubsystem() {}

void UT8SessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            // Bind delegates permanently or as needed
            OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UT8SessionSubsystem::OnCreateSessionComplete);
            OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UT8SessionSubsystem::OnFindSessionsComplete);
            OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UT8SessionSubsystem::OnJoinSessionComplete);
            OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UT8SessionSubsystem::OnDestroySessionComplete);
        }
    }
}

void UT8SessionSubsystem::CreateSession(int32 MaxPlayers, bool bIsLAN, FString MapName, FString TeamMode, FString SessionName)
{
    if (!SessionInterface.IsValid())
    {
        OnSessionCreateComplete_BP.Broadcast(false);
        return;
    }

    // Check for existing session? Optional.

    // Clear previous handle if any
    SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);

    // Add delegate handler
    OnCreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

    // Configure settings
    TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
    SessionSettings->NumPublicConnections = MaxPlayers;
    SessionSettings->bIsLANMatch = bIsLAN;
    SessionSettings->bShouldAdvertise = true;
    SessionSettings->bUsesPresence = !bIsLAN;
    SessionSettings->bAllowJoinInProgress = true;
    SessionSettings->bUseLobbiesIfAvailable = true; // If using Steam Lobbies via Advanced Sessions
    SessionSettings->Set(FName("MAPNAME"), MapName, EOnlineDataAdvertisementType::ViaOnlineService);
    SessionSettings->Set(FName("TEAMMODE"), TeamMode, EOnlineDataAdvertisementType::ViaOnlineService);
    SessionSettings->Set(FName("SESSION_TYPE"), FString("PartyGame"), EOnlineDataAdvertisementType::ViaOnlineService);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
        OnSessionCreateComplete_BP.Broadcast(false);
        return;
    }

    if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), FName(*SessionName), *SessionSettings))
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
        OnSessionCreateComplete_BP.Broadcast(false);
    }
    // Result will be handled in OnCreateSessionComplete callback
}

void UT8SessionSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
    }

    if (bWasSuccessful)
    {
        // Start the session (necessary for it to become joinable)
        SessionInterface->StartSession(SessionName);

        // Open level as listen server
        FString MapNameSetting;
        if (SessionInterface->GetSessionSettings(SessionName)->Get(FName("MAPNAME"), MapNameSetting))
        {
            UGameplayStatics::OpenLevel(GetWorld(), FName(*MapNameSetting), true, "listen");
        }
        else
        {
            // Fallback or error
            UGameplayStatics::OpenLevel(GetWorld(), FName("DefaultLobbyMap"), true, "listen");
        }
    }

    OnSessionCreateComplete_BP.Broadcast(bWasSuccessful);
}

void UT8SessionSubsystem::FindSessions(int32 MaxResults, bool bIsLAN)
{
    if (!SessionInterface.IsValid())
    {
        OnSessionFindComplete_BP.Broadcast(false, {});
        return;
    }

    SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
    OnFindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->MaxSearchResults = MaxResults > 0 ? MaxResults : 100; // Ensure positive max results
    SessionSearch->bIsLanQuery = bIsLAN;
    SessionSearch->QuerySettings.Set(FName("MAPNAME"), true, EOnlineComparisonOp::Equals);
    SessionSearch->QuerySettings.Set(FName("SESSION_TYPE"), FString("PartyGame"), EOnlineComparisonOp::Equals); // Filter for our game type

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
        OnSessionFindComplete_BP.Broadcast(false, {});
        return;
    }

    if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef()))
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
        OnSessionFindComplete_BP.Broadcast(false, {});
    }
    // Results handled in OnFindSessionsComplete
}

void UT8SessionSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
    }

    TArray<FBlueprintSessionResult> BPSessionResults;
    if (bWasSuccessful && SessionSearch.IsValid())
    {
        for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
        {
            if (Result.IsValid()) // Ensure the result is valid
            {
                FBlueprintSessionResult BPResult;
                BPResult.OnlineResult = Result; // Copy the result
                BPSessionResults.Add(BPResult);
            }
        }
    }
    OnSessionFindComplete_BP.Broadcast(bWasSuccessful, BPSessionResults);
}

void UT8SessionSubsystem::JoinSession(const FBlueprintSessionResult& SearchResult)
{
    if (!SessionInterface.IsValid() || !SearchResult.OnlineResult.IsValid())
    {
        OnSessionJoinComplete_BP.Broadcast(false);
        return;
    }

    SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
    OnJoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
    {
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
        OnSessionJoinComplete_BP.Broadcast(false);
        return;
    }

    if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), FName("MyGameSession"), SearchResult.OnlineResult)) // Use a consistent session name or get from settings
    {
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
        OnSessionJoinComplete_BP.Broadcast(false);
    }
    // Result handled in OnJoinSessionComplete
}

void UT8SessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
    }

    bool bSuccess = (Result == EOnJoinSessionCompleteResult::Success);
    if (bSuccess)
    {
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PlayerController)
        {
            FString ConnectString;
            if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
            {
                PlayerController->ClientTravel(ConnectString, TRAVEL_Absolute);
            }
            else
            {
                bSuccess = false; // Failed to get connect string
            }
        }
        else
        {
            bSuccess = false; // Failed to get player controller
        }
    }
    OnSessionJoinComplete_BP.Broadcast(bSuccess);
}

void UT8SessionSubsystem::DestroySession()
{
    if (!SessionInterface.IsValid())
    {
        OnSessionDestroyComplete_BP.Broadcast(false);
        return;
    }

    SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
    OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

    if (!SessionInterface->DestroySession(FName("MyGameSession"))) // Use the correct session name
    {
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
        OnSessionDestroyComplete_BP.Broadcast(false);
    }
    // Result handled in OnDestroySessionComplete
}

void UT8SessionSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
    }
    OnSessionDestroyComplete_BP.Broadcast(bWasSuccessful);
    // Typically navigate back to main menu here if successful
    if (bWasSuccessful)
    {
        UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenuMap"), true);
    }
}