#include "Multi/MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

static const FName SESSION_SEARCH_KEYWORDS(TEXT("SEARCH_KEYWORDS"));

// 디버그용 출력 함수
void PrintString(FString String)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, String);
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
    // OnlineSubsystem과 SessionInterface 초기화 및 delegate 바인딩
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        SessionInterface = OnlineSubsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            // 세션 생성, 검색, 참가 delegate 바인딩
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionsComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnJoinSessionComplete);
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
void UMultiplayerSessionsSubsystem::QuickMatch()
{
    if (!SessionInterface.IsValid())
    {
        PrintString("QuickMatch: SessionInterface가 유효하지 않습니다.");
        return;
    }

    // 세션 검색 객체 생성 및 설정
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
    SessionSearch->MaxSearchResults = 100;

    // "QuickMatch" 키워드로 세션을 필터링
    SessionSearch->QuerySettings.Set(SESSION_SEARCH_KEYWORDS, FString("QuickMatch"), EOnlineComparisonOp::Equals);

    PrintString("QuickMatch: 세션 검색 중...");
    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

// 세션 검색 완료 delegate (Quick Match용)
void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (bWasSuccessful && SessionSearch.IsValid())
    {
        bool bFoundSession = false;
        for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
        {
            FString FoundKeyword;
            if (SearchResult.Session.SessionSettings.Get(SESSION_SEARCH_KEYWORDS, FoundKeyword) && FoundKeyword == "QuickMatch")
            {
                bFoundSession = true;
                PrintString("QuickMatch: 기존 세션 발견. 세션 참가 진행...");
                SessionInterface->JoinSession(0, FName("QuickMatchSession"), SearchResult);
                break;
            }
        }

        if (!bFoundSession)
        {
            PrintString("QuickMatch: 세션을 찾지 못했습니다. 새 세션 생성...");
            CreateQuickMatchSession();
        }
    }
    else
    {
        PrintString("QuickMatch: 세션 검색 실패. CreateQuickMatchSession 호출 안 됨.");
    }
}

// 새로운 Quick Match 세션 생성
void UMultiplayerSessionsSubsystem::CreateQuickMatchSession()
{
    if (!SessionInterface.IsValid())
    {
        PrintString("QuickMatch: SessionInterface가 유효하지 않습니다.");
        return;
    }

    FName SessionName("QuickMatchSession");
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
    SessionSettings.NumPublicConnections = 4; // 예: 2vs2 모드 -> 4명
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bAllowJoinInProgress = true;

    // QuickMatch임을 나타내는 키워드 설정
    SessionSettings.Set(SESSION_SEARCH_KEYWORDS, FString("QuickMatch"), EOnlineDataAdvertisementType::ViaOnlineService);

    // 랜덤 맵 및 게임 모드 선택
    FString SelectedMap = GetRandomMap();
    FString SelectedMode = GetRandomMode();
    SessionSettings.Set(FName("MapName"), SelectedMap, EOnlineDataAdvertisementType::ViaOnlineService);
    SessionSettings.Set(FName("GameMode"), SelectedMode, EOnlineDataAdvertisementType::ViaOnlineService);

    PrintString(FString::Printf(TEXT("QuickMatch: 세션 생성 - 맵: %s, 모드: %s"), *SelectedMap, *SelectedMode));
    SessionInterface->CreateSession(0, SessionName, SessionSettings);
}

// 세션 생성 완료 delegate (Quick Match 또는 다른 세션용)
void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PrintString("QuickMatch: 세션 생성 성공.");
        if (SessionName == FName("QuickMatchSession"))
        {
            // 실제 환경에서는 플레이어 조인 이벤트로 플레이어 수를 체크해야 함
            int32 CurrentPlayers = GetCurrentPlayerCount();
            int32 RequiredPlayers = 4;

            if (CurrentPlayers >= RequiredPlayers)
            {
                OnQuickMatchReady();
            }
            else
            {
                PrintString("QuickMatch: 세션 생성됨. 추가 플레이어 대기 중...");
            }
        }
    }
    else
    {
        PrintString("QuickMatch: 세션 생성 실패.");
    }
}

// 세션 참가 완료 delegate
void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (SessionInterface.IsValid())
    {
        FString ConnectString;
        if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
        {
            PrintString("QuickMatch: 세션 참가 성공.");
            APlayerController* PC = GetWorld()->GetFirstPlayerController();
            if (PC)
            {
                PC->ClientTravel(ConnectString, TRAVEL_Absolute);
            }
        }
        else
        {
            PrintString("QuickMatch: 세션 참가 후 접속 문자열 획득 실패.");
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// 게임 시작 알림 및 레벨 이동 (Quick Match)
//////////////////////////////////////////////////////////////////////////

// 모든 참가자가 모였을 때 호출 (Quick Match)
void UMultiplayerSessionsSubsystem::OnQuickMatchReady()
{
    // 게임 시작 알림 출력
    PrintString("모든 플레이어가 모였습니다! 게임 시작 5초 후 레벨 이동합니다.");

    // 5초 후 게임 레벨로 이동하는 타이머 설정
    FTimerHandle TimerHandle;
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMultiplayerSessionsSubsystem::TravelToGameLevel, 5.f, false);
    }
}

// 타이머가 만료되면 게임 레벨로 이동
void UMultiplayerSessionsSubsystem::TravelToGameLevel()
{
    // 세션 생성 시 설정한 맵 이름을 사용할 수도 있음. 여기서는 예시로 고정된 경로 사용.
    FString GameMap = "/Game/Maps/GameMap";
    if (GetWorld())
    {
        PrintString("QuickMatch: 게임 레벨로 이동합니다.");
        GetWorld()->ServerTravel(GameMap + "?listen");
    }
}

//////////////////////////////////////////////////////////////////////////
// Room List / Browse Game 관련 기능
//////////////////////////////////////////////////////////////////////////

// Room List의 Create Game 버튼을 눌렀을 때 호출 (로비 세션 생성)
void UMultiplayerSessionsSubsystem::CreateGameSession(FString ServerName)
{
    // TODO: 세션 생성 후 로비 레벨로 이동
    PrintString("CreateGameSession: 기능 구현 필요");
}

// Room List에서 세션 선택 후 Join Game 버튼을 눌렀을 때 호출
void UMultiplayerSessionsSubsystem::JoinGameSession()
{
    // TODO: 선택한 세션 여부 확인 및 인원 수 체크 후 참가 처리
    //       - 선택된 세션이 없으면 "세션 선택해주세요" 메시지 출력
    //       - 선택된 세션이 가득 찼으면 "세션이 가득 찼습니다" 메시지 출력
    //       - 그렇지 않으면 세션 참가 진행
    PrintString("JoinGameSession: 기능 구현 필요");
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
    // TODO: 랜덤 맵 선택 로직
    return FString("RandomMap");
}

FString UMultiplayerSessionsSubsystem::GetRandomMode()
{
    // TODO: 랜덤 모드 선택 로직 (예: "2vs2", "Solo", "AIMatch" 등)
    return FString("RandomMode");
}

int32 UMultiplayerSessionsSubsystem::GetCurrentPlayerCount()
{
    // TODO: 현재 접속 플레이어 수 반환 로직 구현
    return 0;
}
