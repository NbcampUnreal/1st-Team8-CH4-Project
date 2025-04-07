#include "Multi/MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"

static const FName SESSION_SEARCH_KEYWORDS(TEXT("SEARCH_KEYWORDS"));

// 디버그용 출력 함수
void PrintString(FString String)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, String);
    }
}

//////////////////////////////////////////////////////////////////////////
// 생성자, 초기화, 해제
//////////////////////////////////////////////////////////////////////////

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // OnlineSubsystem과 SessionInterface 초기화 및 delegate 바인딩
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        SessionInterface = OnlineSubsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            // 세션 생성, 검색, 참가 delegate 바인딩
            //SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnCreateQuickSessionComplete);
            //SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnFindQuickSessionsComplete);
            //SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnJoinQuickSessionComplete);
			//SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnDestroySessionComplete);
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionsComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnJoinSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnDestroySessionComplete);
        }
    }
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
    // TODO: 필요한 정리 코드
}

//////////////////////////////////////////////////////////////////////////
// Quick Match 관련 기능
//////////////////////////////////////////////////////////////////////////

// Quick Match 버튼을 눌렀을 때 호출
//void UMultiplayerSessionsSubsystem::QuickMatch()
//{
//    if (!SessionInterface.IsValid())
//    {
//        PrintString("QuickMatch: SessionInterface가 유효하지 않습니다.");
//        return;
//    }
//
//    // 세션 검색 객체 생성 및 설정
//    SessionSearch = MakeShareable(new FOnlineSessionSearch());
//    SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
//    SessionSearch->MaxSearchResults = 100;
//
//    // "QuickMatch" 키워드로 세션을 필터링
//    SessionSearch->QuerySettings.Set(SESSION_SEARCH_KEYWORDS, FString("QuickMatch"), EOnlineComparisonOp::Equals);
//
//    PrintString("QuickMatch: Searching for sessions...");
//    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
//}
//
//// 세션 검색 완료 delegate (Quick Match용)
//void UMultiplayerSessionsSubsystem::OnFindQuickSessionsComplete(bool bWasSuccessful)
//{
//    if (bWasSuccessful && SessionSearch.IsValid())
//    {
//        bool bFoundSession = false;
//        for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
//        {
//            FString FoundKeyword;
//            if (SearchResult.Session.SessionSettings.Get(SESSION_SEARCH_KEYWORDS, FoundKeyword) && FoundKeyword == "QuickMatch")
//            {
//                bFoundSession = true;
//                PrintString("QuickMatch: Existing session found. Join session...");
//                SessionInterface->JoinSession(0, FName("QuickMatchSession"), SearchResult);
//                break;
//            }
//        }
//
//        if (!bFoundSession)
//        {
//            PrintString("QuickMatch: Failed to find session, create new session...");
//            CreateQuickMatchSession();
//        }
//    }
//    else
//    {
//        PrintString("QuickMatch: 세션 검색 실패. CreateQuickMatchSession 호출 안 함.");
//    }
//}
//
//// 새로운 Quick Match 세션 생성
//void UMultiplayerSessionsSubsystem::CreateQuickMatchSession()
//{
//    if (!SessionInterface.IsValid())
//    {
//        PrintString("QuickMatch: SessionInterface가 유효하지 않습니다.");
//        return;
//    }
//
//    FName SessionName("QuickMatchSession");
//    FOnlineSessionSettings SessionSettings;
//    SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
//
//    FGameModeData SelectedModeData = GetRandomMode();
//    FString SelectedMap = GetRandomMap();
//
//    QuickMatchSelectedMap = SelectedMap;
//    QuickMatchRequiredPlayers = SelectedModeData.RequiredPlayers;
//
//    SessionSettings.NumPublicConnections = SelectedModeData.RequiredPlayers; // 예: 2vs2 모드 -> 4명
//    SessionSettings.bShouldAdvertise = true;
//    SessionSettings.bUsesPresence = true;
//    SessionSettings.bAllowJoinInProgress = true;
//
//    // QuickMatch임을 나타내는 키워드 설정
//    SessionSettings.Set(SESSION_SEARCH_KEYWORDS, FString("QuickMatch"), EOnlineDataAdvertisementType::ViaOnlineService);
//
//    SessionSettings.Set(FName("MapName"), SelectedMap, EOnlineDataAdvertisementType::ViaOnlineService);
//    SessionSettings.Set(FName("GameMode"), SelectedModeData.ModeName, EOnlineDataAdvertisementType::ViaOnlineService);
//
//    PrintString(FString::Printf(TEXT("QuickMatch: 세션 생성 - 맵: %s, 모드: %s, 인원: %d"), *SelectedMap, *SelectedModeData.ModeName, SelectedModeData.RequiredPlayers));
//    SessionInterface->CreateSession(0, SessionName, SessionSettings);
//}
//
//// 세션 생성 완료 delegate (Quick Match 또는 다른 세션용)
//void UMultiplayerSessionsSubsystem::OnCreateQuickSessionComplete(FName SessionName, bool bWasSuccessful)
//{
//    if (bWasSuccessful)
//    {
//        PrintString("QuickMatch: Successfully created session.");
//        if (SessionName == FName("QuickMatchSession"))
//        {
//            // 기존 즉시 체크 대신 폴링 타이머를 시작
//            if (GetWorld())
//            {
//                GetWorld()->GetTimerManager().SetTimer(QuickMatchPollTimerHandle, this, &UMultiplayerSessionsSubsystem::PollQuickMatchPlayerCount, 1.f, true);
//            }
//        }
//    }
//    else
//    {
//        PrintString("QuickMatch: Failed to create session.");
//    }
//}
//
//// 세션 참가 완료 delegate
//void UMultiplayerSessionsSubsystem::OnJoinQuickSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
//{
//    if (SessionInterface.IsValid())
//    {
//        FString ConnectString;
//        if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
//        {
//            PrintString("QuickMatch: Successful session participation.");
//            APlayerController* PC = GetWorld()->GetFirstPlayerController();
//            if (PC)
//            {
//                PC->ClientTravel(ConnectString, TRAVEL_Absolute);
//            }
//        }
//        else
//        {
//            PrintString("QuickMatch: Failed to obtain access string after session participation.");
//        }
//    }
//}
//
////////////////////////////////////////////////////////////////////////////
//// 게임 시작 알림 및 레벨 이동 (Quick Match)
////////////////////////////////////////////////////////////////////////////
//
//// 모든 참가자가 모였을 때 호출 (Quick Match)
//void UMultiplayerSessionsSubsystem::OnQuickMatchReady()
//{
//    // 게임 시작 알림 출력
//    PrintString("모든 플레이어가 모였습니다! 게임 시작 5초 후 레벨 이동합니다.");
//
//    // 5초 후 게임 레벨로 이동하는 타이머 설정
//    FTimerHandle TimerHandle;
//    if (GetWorld())
//    {
//        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMultiplayerSessionsSubsystem::TravelToGameLevel, 5.f, false);
//    }
//}
//
//// 타이머가 만료되면 게임 레벨로 이동
//void UMultiplayerSessionsSubsystem::TravelToGameLevel()
//{
//    FString GameMap = QuickMatchSelectedMap.IsEmpty() ? "/Game/PlatformFighterKit/Maps/Levels/DefaultLevel" : QuickMatchSelectedMap;
//
//    if (GetWorld())
//    {
//        PrintString("QuickMatch: 게임 레벨로 이동합니다.");
//        GetWorld()->ServerTravel(GameMap + "?listen");
//    }
//}

//////////////////////////////////////////////////////////////////////////
// Room List / Browse Game 관련 기능
//////////////////////////////////////////////////////////////////////////

void UMultiplayerSessionsSubsystem::CreateGameSession(FString ServerName)
{
    if (!SessionInterface.IsValid())
    {
        PrintString("CreateGameSession: SessionInterface가 유효하지 않습니다.");
        return;
    }

    FName SessionName("LobbySession");
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
    SessionSettings.bIsDedicated = false;
    int32 LobbyRequiredPlayers = 4;
    SessionSettings.NumPublicConnections = LobbyRequiredPlayers;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bUseLobbiesIfAvailable = true;
    SessionSettings.bAllowJoinViaPresence = true;

    // QuickMatch와 구분하기 위해 "Lobby" 키워드 사용
    SessionSettings.Set(SESSION_SEARCH_KEYWORDS, ServerName, EOnlineDataAdvertisementType::ViaOnlineService);
    // 서버 이름 설정
    SessionSettings.Set(FName("ServerName"), ServerName, EOnlineDataAdvertisementType::ViaOnlineService);
    // 요구 플레이어 수를 세션 설정에 추가 (추후 비교용)
    SessionSettings.Set(FName("RequiredPlayers"), LobbyRequiredPlayers, EOnlineDataAdvertisementType::ViaOnlineService);

    PrintString(FString::Printf(TEXT("CreateGameSession: 로비 세션 생성 중, 서버 이름: %s"), *ServerName));
    SessionInterface->CreateSession(0, SessionName, SessionSettings);
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool WasSuccessful)
{
    PrintString(FString::Printf(TEXT("OnCreateSessionComplete: %d"), WasSuccessful));

    ServerCreateDel.Broadcast(WasSuccessful);

    if (WasSuccessful)
    {
        GetWorld()->ServerTravel("/Game/Session/TestLobby?listen");
    }
}

// Room List에서 세션 선택 후 Join Game 버튼을 눌렀을 때 호출
void UMultiplayerSessionsSubsystem::FindSession(FString ServerName)
{
    PrintString("FindServer");

	if (ServerName.IsEmpty())
	{
		PrintString("Server name cannot be empty!");
		ServerJoinDel.Broadcast(false);
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	bool IsLAN = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		IsLAN = true;
	}
	SessionSearch->bIsLanQuery = IsLAN;
	SessionSearch->MaxSearchResults = 9999;
	SessionSearch->QuerySettings.Set(SESSION_SEARCH_KEYWORDS, true, EOnlineComparisonOp::Equals);

	ServerNameToFind = ServerName;

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());

}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool WasSuccessful)
{
    if (!WasSuccessful) return;
    if (ServerNameToFind.IsEmpty()) return;

    TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
    FOnlineSessionSearchResult* CorrectResult = 0;

    if (Results.Num() > 0)
    {
        FString Msg = FString::Printf(TEXT("%d sessions found."), Results.Num());
        PrintString(Msg);

        for (FOnlineSessionSearchResult Result : Results)
        {
            if (Result.IsValid())
            {
                FString ServerName = "No-name";
                Result.Session.SessionSettings.Get(FName("SERVER_NAME"), ServerName);

                if (ServerName.Equals(ServerNameToFind))
                {
                    CorrectResult = &Result;
                    FString Msg2 = FString::Printf(TEXT("Found server with name: %s"), *ServerName);
                    PrintString(Msg2);
                    break;
                }
            }
        }

        if (CorrectResult)
        {
            SessionInterface->JoinSession(0, MySessionName, *CorrectResult);
        }
        else
        {
            PrintString(FString::Printf(TEXT("Couldn't find server: %s"), *ServerNameToFind));
            ServerNameToFind = "";
            ServerJoinDel.Broadcast(false);
        }
    }
    else
    {
        PrintString("Zero sessions found.");
        ServerJoinDel.Broadcast(false);
    }
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    ServerJoinDel.Broadcast(Result == EOnJoinSessionCompleteResult::Success);

    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        FString Msg = FString::Printf(TEXT("Successfully joined session %s"), *SessionName.ToString());
        PrintString(Msg);

        FString Address = "";
        bool Success = SessionInterface->GetResolvedConnectString(MySessionName, Address);
        if (Success)
        {
            PrintString(FString::Printf(TEXT("Address: %s"), *Address));
            APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
            if (PlayerController)
            {
                PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
            }
        }
        else
        {
            PrintString("GetResolvedConnectString returned false!");
        }
    }
    else
    {
        PrintString("OnJoinSessionComplete failed");
    }
}

void UMultiplayerSessionsSubsystem::DestroySession(FName SessionName)
{
    if (!SessionInterface.IsValid())
    {
        PrintString("DestroySession: SessionInterface가 유효하지 않습니다.");
        return;
    }

    PrintString(FString::Printf(TEXT("DestroySession: 세션 (%s) 파괴 요청 중..."), *SessionName.ToString()));
    SessionInterface->DestroySession(SessionName);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PrintString(FString::Printf(TEXT("DestroySession: 세션 (%s) 파괴 성공"), *SessionName.ToString()));
    }
    else
    {
        PrintString(FString::Printf(TEXT("DestroySession: 세션 (%s) 파괴 실패"), *SessionName.ToString()));
    }
}

//////////////////////////////////////////////////////////////////////////
// 로비에서 게임 시작 관련 기능
//////////////////////////////////////////////////////////////////////////

// 로비 레벨의 Game Start 버튼을 눌렀을 때 호출
void UMultiplayerSessionsSubsystem::StartGameFromLobby()
{
    // TODO: 현재 플레이어 수와 해당 모드의 요구 인원을 확인하여
    //       조건이 맞으면 게임 시작, 아니면 알림 출력
    PrintString("StartGameFromLobby: 기능 구현 필요");
}

//////////////////////////////////////////////////////////////////////////
// 방장 기능 - 플레이어 추방
//////////////////////////////////////////////////////////////////////////

// 방장이 선택한 플레이어를 추방할 때 호출
void UMultiplayerSessionsSubsystem::KickPlayer(const FUniqueNetIdRepl& PlayerId)
{
    // TODO: 선택한 플레이어를 세션에서 추방하는 로직 구현
    PrintString("KickPlayer: 기능 구현 필요");
}

//////////////////////////////////////////////////////////////////////////
// 유틸리티 함수: 랜덤 맵, 랜덤 모드, 현재 플레이어 수 확인 등
//////////////////////////////////////////////////////////////////////////

FString UMultiplayerSessionsSubsystem::GetRandomMap()
{
    // 검색할 폴더 경로 (Content/ 아래 경로는 "/Game/"으로 시작)
    FString FolderPath = "/Game/PlatformFighterKit/Maps/Levels";
    TArray<FAssetData> AssetList;

    // AssetRegistryModule 로드 및 IAssetRegistry 참조 획득
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    // 폴더 내 모든 자산(하위 폴더 포함) 검색
    AssetRegistry.GetAssetsByPath(FName(*FolderPath), AssetList, true);

    // 레벨(월드)만 필터링 (월드 클래스는 "World")
    TArray<FAssetData> WorldAssets;
    for (const FAssetData& Asset : AssetList)
    {
        if (Asset.AssetClass == FName("World"))
        {
            WorldAssets.Add(Asset);
        }
    }

    // 레벨이 하나라도 있다면 랜덤으로 선택
    if (WorldAssets.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, WorldAssets.Num() - 1);
        FString MapPath = WorldAssets[RandomIndex].ObjectPath.ToString();
        return MapPath;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No level found in folder: %s"), *FolderPath);
        // 기본값 지정 (폴더 내에 레벨이 없을 경우)
        return FString("/Game/PlatformFighterKit/Maps/Levels/DefaultLevel");
    }
}

FGameModeData UMultiplayerSessionsSubsystem::GetRandomMode()
{
    TArray<FGameModeData> Modes;

    Modes.Add({ "2vs2vs2", 6 });
    Modes.Add({ "3vs3", 6 });
    Modes.Add({ "Solo", 4 });
    Modes.Add({ "2Human_vs_2AI", 2 });
    Modes.Add({ "1v1vAIvAI", 2 });

    int32 RandomIndex = FMath::RandRange(0, Modes.Num() - 1);
    return Modes[RandomIndex];
}

int32 UMultiplayerSessionsSubsystem::GetCurrentPlayerCount()
{
    if (SessionInterface.IsValid())
    {
        // "QuickMatchSession" is the session name used for Quick Match sessions.
        FNamedOnlineSession* Session = SessionInterface->GetNamedSession(FName("QuickMatchSession"));
        if (Session)
        {
            return Session->RegisteredPlayers.Num();
        }
    }
    return 0;
}

//void UMultiplayerSessionsSubsystem::PollQuickMatchPlayerCount()
//{
//    int32 CurrentPlayers = GetCurrentPlayerCount();
//    if (CurrentPlayers >= QuickMatchRequiredPlayers)
//    {
//        if (GetWorld())
//        {
//            GetWorld()->GetTimerManager().ClearTimer(QuickMatchPollTimerHandle);
//        }
//        OnQuickMatchReady();
//    }
//}
