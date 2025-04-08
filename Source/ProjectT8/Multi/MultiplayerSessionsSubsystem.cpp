#include "Multi/MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Kismet/GameplayStatics.h"

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
        ServerCreateDel.Broadcast(false);
        return;
    }
    if (ServerName.IsEmpty())
    {
        PrintString("CreateGameSession: Session Name cannot be Empty");
        ServerCreateDel.Broadcast(false);
        return;
    }

    FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(MySessionName);
    if (ExistingSession)
    {
        FString Msg = FString::Printf(TEXT("Session with name %s already exists, destroying it."), *MySessionName.ToString());
        PrintString(Msg);
        CreateServerAfterDestroy = true;
        DestroyServerName = ServerName;
        SessionInterface->DestroySession(MySessionName);
        return;
    }

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

    SessionSettings.Set(FName("SERVER_NAME"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    PrintString(FString::Printf(TEXT("CreateGameSession: Creating lobby session, Server name: %s"), *ServerName));
    if (!SessionInterface->CreateSession(0, FName(NAME_GameSession), SessionSettings))
    {
        PrintString("CreateGameSession: CreateSession 호출 자체 실패.");
        ServerCreateDel.Broadcast(false);
        // 필요한 경우 CreateServerAfterDestroy 등 상태 초기화
        CreateServerAfterDestroy = false;
        DestroyServerName = TEXT("");
    }
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

    if (!SessionInterface.IsValid())
    {
        PrintString("FindSession: SessionInterface가 유효하지 않습니다.");
        // ServerJoinDel.Broadcast(false); // 여기서 Join 실패를 알리는 것은 적절하지 않음
        return;
    }

    if (ServerName.IsEmpty())
    {
        PrintString("FindSession: 서버 이름이 비어있습니다!");
        // ServerJoinDel.Broadcast(false); // 여기서 Join 실패를 알리는 것은 적절하지 않음
        return;
    }

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
    if (!SessionSearch.IsValid())
    {
        PrintString("FindSession: 세션 검색 객체 생성 실패.");
        return;
    }

    SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSearch->MaxSearchResults = 20;

	ServerNameToFind = ServerName;

    PrintString(FString::Printf(TEXT("FindSession: 온라인 서브시스템에 세션 검색 요청 (IsLAN: %d)..."), SessionSearch->bIsLanQuery));

    if (!SessionInterface->FindSessions(0, SessionSearch.ToSharedRef()))
    {
        // FindSessions 호출 자체가 실패한 경우
        PrintString("FindSession: FindSessions 호출 자체 실패.");
        ServerNameToFind = ""; // 검색 상태 초기화
        // ServerJoinDel.Broadcast(false); // 여기서 Join 실패를 알리는 것은 적절하지 않음
    }

}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool WasSuccessful)
{
    PrintString(FString::Printf(TEXT("OnFindSessionsComplete: 검색 성공 여부: %d"), WasSuccessful));

    // 검색 자체가 실패했거나, 검색 객체가 유효하지 않으면 더 이상 진행하지 않음
    if (!WasSuccessful || !SessionSearch.IsValid())
    {
        ServerNameToFind = ""; // 검색 상태 초기화
        // ServerJoinDel.Broadcast(false); // 여기서 Join 실패를 알리는 것은 적절하지 않음
        PrintString("OnFindSessionsComplete: 세션 검색 실패 또는 검색 객체 무효.");
        return;
    }

    // 검색 결과를 로컬 변수에 복사 (SessionSearch는 다른 검색에 재사용될 수 있으므로)
    TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

    PrintString(FString::Printf(TEXT("OnFindSessionsComplete: %d개의 세션 찾음."), SearchResults.Num()));

    // 찾으려는 서버 이름이 설정되어 있는지 확인 (FindSession 호출 후 변경되지 않았는지)
    if (ServerNameToFind.IsEmpty())
    {
        PrintString("OnFindSessionsComplete: 찾으려는 서버 이름이 지정되지 않았습니다.");
        return;
    }

    FOnlineSessionSearchResult* FoundResultPtr = nullptr;

    // 검색 결과를 순회하며 원하는 서버 이름(SERVER_NAME)을 가진 세션 찾기
    for (FOnlineSessionSearchResult& Result : SearchResults)
    {
        if (Result.IsValid())
        {
            FString AdvertisedServerName = TEXT("No-name"); // 기본값
            // 세션 설정에서 "SERVER_NAME" 키의 값을 가져옴
            Result.Session.SessionSettings.Get(FName("SERVER_NAME"), AdvertisedServerName);

            PrintString(FString::Printf(TEXT(" - 찾은 세션 이름: %s"), *AdvertisedServerName));

            // 찾으려는 이름과 광고된 이름이 일치하는지 확인
            if (AdvertisedServerName.Equals(ServerNameToFind, ESearchCase::IgnoreCase)) // 대소문자 무시 비교
            {
                PrintString(FString::Printf(TEXT("OnFindSessionsComplete: 원하는 서버 '%s' 찾음! 참가 시도..."), *ServerNameToFind));
                FoundResultPtr = &Result; // 포인터 저장
                break; // 찾았으므로 반복 중단
            }
        }
    }

    // 일치하는 세션을 찾았는지 확인
    if (FoundResultPtr)
    {
        // 세션 참가 시작 (비동기)
        if (!SessionInterface->JoinSession(0, MySessionName, *FoundResultPtr))
        {
            // JoinSession 호출 자체가 실패한 경우
            PrintString(FString::Printf(TEXT("OnFindSessionsComplete: '%s' 서버 JoinSession 호출 자체 실패."), *ServerNameToFind));
            ServerNameToFind = ""; // 검색 상태 초기화
            ServerJoinDel.Broadcast(false); // 참가 시도 결과를 알림
        }
        // JoinSession 호출 성공 시, 결과는 OnJoinSessionComplete 콜백에서 처리됨
    }
    else
    {
        // 검색 결과 중에서 원하는 이름의 서버를 찾지 못한 경우
        PrintString(FString::Printf(TEXT("OnFindSessionsComplete: 검색 결과 중 '%s' 이름의 서버를 찾지 못했습니다."), *ServerNameToFind));
        // ServerJoinDel.Broadcast(false); // 여기서 Join 실패를 알리는 것은 적절하지 않음 (찾지 못한 것일 뿐)
    }

    // 검색 상태 초기화 (다음 검색을 위해)
    ServerNameToFind = "";
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    // 참가 시도 결과 브로드캐스트 (성공/실패 모두 여기서 처리)
    ServerJoinDel.Broadcast(Result == EOnJoinSessionCompleteResult::Success);

    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        PrintString(FString::Printf(TEXT("OnJoinSessionComplete: 세션 '%s' 참가 성공!"), *SessionName.ToString()));

        FString ConnectAddress = TEXT("");
        // 참가한 세션의 접속 주소(IP 또는 OSS별 식별자) 가져오기
        if (SessionInterface.IsValid() && SessionInterface->GetResolvedConnectString(SessionName, ConnectAddress))
        {
            PrintString(FString::Printf(TEXT("OnJoinSessionComplete: 접속 주소 확인: %s"), *ConnectAddress));
            APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
            if (PlayerController)
            {
                PrintString(FString::Printf(TEXT("OnJoinSessionComplete: ClientTravel 실행 -> %s"), *ConnectAddress));
                // 클라이언트를 해당 주소의 서버(호스트)로 이동시킴
                PlayerController->ClientTravel(ConnectAddress, ETravelType::TRAVEL_Absolute);
            }
            else
            {
                PrintString("OnJoinSessionComplete: PlayerController 가져오기 실패.");
            }
        }
        else
        {
            PrintString("OnJoinSessionComplete: GetResolvedConnectString 실패!");
            // 필요하다면 여기서도 ServerJoinDel.Broadcast(false)를 다시 호출할 수 있지만, 이미 위에서 처리됨.
        }
    }
    else // 참가 실패 처리
    {
        // 실패 이유에 따른 로그 출력 (더 상세하게)
        FString FailureReason;
        switch (Result)
        {
        case EOnJoinSessionCompleteResult::SessionIsFull: FailureReason = TEXT("Session is full"); break;
        case EOnJoinSessionCompleteResult::SessionDoesNotExist: FailureReason = TEXT("Session does not exist"); break;
        case EOnJoinSessionCompleteResult::AlreadyInSession: FailureReason = TEXT("Already in session"); break;
            // ... 기타 다른 실패 이유들 ...
        default: FailureReason = TEXT("Unknown error"); break;
        }
        PrintString(FString::Printf(TEXT("OnJoinSessionComplete: 세션 참가 실패. 이유: %s"), *FailureReason));
    }
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
    if (!SessionInterface.IsValid())
    {
        PrintString("DestroySession: SessionInterface가 유효하지 않습니다.");
        ServerDestroyDel.Broadcast(false); // 실패 브로드캐스트
        return;
    }

    bIsHost = true;
    PrintString(FString::Printf(TEXT("DestroySession: 세션 (%s) 파괴 요청 중..."), *MySessionName.ToString()));
    if (!SessionInterface->DestroySession(MySessionName))
    {
        PrintString("DestroySession: DestroySession 호출 자체 실패.");
        bIsHost = false;
        ServerDestroyDel.Broadcast(false); // 실패 브로드캐스트
    }
}

void UMultiplayerSessionsSubsystem::LeaveSession()
{
    if (!SessionInterface.IsValid())
    {
        PrintString("LeaveSession (Client): SessionInterface가 유효하지 않습니다.");
        ClientLeaveSessionDel.Broadcast(false);
        return;
    }

    bIsHost = false; // 클라이언트가 나가기를 시작했음을 표시 (콜백에서 구분하기 위한 예시)
    PrintString(FString::Printf(TEXT("LeaveSession (Client): 세션 (%s) 나가기 요청 중..."), *MySessionName.ToString()));
    if (!SessionInterface->DestroySession(MySessionName)) // 동일한 DestroySession 함수 사용
    {
        PrintString("LeaveSession (Client): DestroySession 호출 자체 실패.");
        ClientLeaveSessionDel.Broadcast(false);
    }
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    PrintString(FString::Printf(TEXT("OnDestroySessionComplete: 세션 이름: %s, 성공 여부: %d"), *SessionName.ToString(), bWasSuccessful));

    if (bIsHost) // 호스트가 시작한 파괴 작업의 콜백인 경우
    {
        PrintString("OnDestroySessionComplete: Session destruction complete on host");
        ServerDestroyDel.Broadcast(bWasSuccessful);
        if (bWasSuccessful)
        {
            PrintString(FString::Printf(TEXT("호스트: 세션 '%s' 성공적으로 파괴됨."), *SessionName.ToString()));
            // 호스트는 PersonalRoom 등으로 이동 (델리게이트 받은 곳에서 처리)
            if (CreateServerAfterDestroy)
            {
                PrintString(FString::Printf(TEXT("호스트: 이전 세션 파괴 완료, 새 세션 '%s' 생성 시도."), *DestroyServerName));
                CreateServerAfterDestroy = false; // 플래그 리셋
                FString ServerNameToCreate = DestroyServerName; // 로컬 변수에 복사
                DestroyServerName = TEXT(""); // 상태 변수 리셋
                CreateGameSession(ServerNameToCreate); // *** 여기서 재생성 호출 ***
            }
            else
            {
                GetWorld()->ServerTravel("/Game/Session/PersonalRoom");
            }
        }
        else
        {
            PrintString(FString::Printf(TEXT("호스트: 세션 '%s' 파괴 실패."), *SessionName.ToString()));
        }
    }
    else // 클라이언트가 시작한 나가기 작업의 콜백인 경우
    {
        PrintString("OnDestroySessionComplete: 클라이언트의 세션 나가기 완료됨.");
        ClientLeaveSessionDel.Broadcast(bWasSuccessful);
        if (bWasSuccessful)
        {
            PrintString(FString::Printf(TEXT("클라이언트: 세션 '%s'에서 성공적으로 나감."), *SessionName.ToString()));
            GetWorld()->ServerTravel("/Game/Session/PersonalRoom");
            // *** 클라이언트: 여기서 메인 메뉴 레벨로 이동 ***
            // 예: 델리게이트를 받은 UI 위젯 등에서 레벨 이동 실행
            // APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
            // if (PC)
            // {
            //     PC->ClientTravel(TEXT("/Game/Session/PersonalRoom"), ETravelType::TRAVEL_Absolute);
            // }
            // 또는 콘솔 명령 실행:
            // GEngine->Exec(GetWorld(), TEXT("Disconnect"));
            // GEngine->Exec(GetWorld(), TEXT("open /Game/Levels/MainMenuLevelName")); // 레벨 경로 확인
        }
        else
        {
            PrintString(FString::Printf(TEXT("클라이언트: 세션 '%s' 나가기 실패."), *SessionName.ToString()));
        }
    }
    bIsHost = false; // 상태 초기화 (선택적)
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
