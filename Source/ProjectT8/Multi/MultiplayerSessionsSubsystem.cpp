#include "Multi/MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

static const FName SESSION_SEARCH_KEYWORDS(TEXT("SEARCH_KEYWORDS"));

// ����׿� ��� �Լ�
void PrintString(FString String)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, String);
    }
}

//////////////////////////////////////////////////////////////////////////
// ������, �ʱ�ȭ, ����
//////////////////////////////////////////////////////////////////////////

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    // OnlineSubsystem�� SessionInterface �ʱ�ȭ �� delegate ���ε�
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        SessionInterface = OnlineSubsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            // ���� ����, �˻�, ���� delegate ���ε�
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionsComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnJoinSessionComplete);
        }
    }
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
    // TODO: �ʿ��� ���� �ڵ�
}

//////////////////////////////////////////////////////////////////////////
// Quick Match ���� ���
//////////////////////////////////////////////////////////////////////////

// Quick Match ��ư�� ������ �� ȣ��
void UMultiplayerSessionsSubsystem::QuickMatch()
{
    if (!SessionInterface.IsValid())
    {
        PrintString("QuickMatch: SessionInterface�� ��ȿ���� �ʽ��ϴ�.");
        return;
    }

    // ���� �˻� ��ü ���� �� ����
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
    SessionSearch->MaxSearchResults = 100;

    // "QuickMatch" Ű����� ������ ���͸�
    SessionSearch->QuerySettings.Set(SESSION_SEARCH_KEYWORDS, FString("QuickMatch"), EOnlineComparisonOp::Equals);

    PrintString("QuickMatch: ���� �˻� ��...");
    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

// ���� �˻� �Ϸ� delegate (Quick Match��)
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
                PrintString("QuickMatch: ���� ���� �߰�. ���� ���� ����...");
                SessionInterface->JoinSession(0, FName("QuickMatchSession"), SearchResult);
                break;
            }
        }

        if (!bFoundSession)
        {
            PrintString("QuickMatch: ������ ã�� ���߽��ϴ�. �� ���� ����...");
            CreateQuickMatchSession();
        }
    }
    else
    {
        PrintString("QuickMatch: ���� �˻� ����. CreateQuickMatchSession ȣ�� �� ��.");
    }
}

// ���ο� Quick Match ���� ����
void UMultiplayerSessionsSubsystem::CreateQuickMatchSession()
{
    if (!SessionInterface.IsValid())
    {
        PrintString("QuickMatch: SessionInterface�� ��ȿ���� �ʽ��ϴ�.");
        return;
    }

    FName SessionName("QuickMatchSession");
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
    SessionSettings.NumPublicConnections = 4; // ��: 2vs2 ��� -> 4��
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bAllowJoinInProgress = true;

    // QuickMatch���� ��Ÿ���� Ű���� ����
    SessionSettings.Set(SESSION_SEARCH_KEYWORDS, FString("QuickMatch"), EOnlineDataAdvertisementType::ViaOnlineService);

    // ���� �� �� ���� ��� ����
    FString SelectedMap = GetRandomMap();
    FString SelectedMode = GetRandomMode();
    SessionSettings.Set(FName("MapName"), SelectedMap, EOnlineDataAdvertisementType::ViaOnlineService);
    SessionSettings.Set(FName("GameMode"), SelectedMode, EOnlineDataAdvertisementType::ViaOnlineService);

    PrintString(FString::Printf(TEXT("QuickMatch: ���� ���� - ��: %s, ���: %s"), *SelectedMap, *SelectedMode));
    SessionInterface->CreateSession(0, SessionName, SessionSettings);
}

// ���� ���� �Ϸ� delegate (Quick Match �Ǵ� �ٸ� ���ǿ�)
void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PrintString("QuickMatch: ���� ���� ����.");
        if (SessionName == FName("QuickMatchSession"))
        {
            // ���� ȯ�濡���� �÷��̾� ���� �̺�Ʈ�� �÷��̾� ���� üũ�ؾ� ��
            int32 CurrentPlayers = GetCurrentPlayerCount();
            int32 RequiredPlayers = 4;

            if (CurrentPlayers >= RequiredPlayers)
            {
                OnQuickMatchReady();
            }
            else
            {
                PrintString("QuickMatch: ���� ������. �߰� �÷��̾� ��� ��...");
            }
        }
    }
    else
    {
        PrintString("QuickMatch: ���� ���� ����.");
    }
}

// ���� ���� �Ϸ� delegate
void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (SessionInterface.IsValid())
    {
        FString ConnectString;
        if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
        {
            PrintString("QuickMatch: ���� ���� ����.");
            APlayerController* PC = GetWorld()->GetFirstPlayerController();
            if (PC)
            {
                PC->ClientTravel(ConnectString, TRAVEL_Absolute);
            }
        }
        else
        {
            PrintString("QuickMatch: ���� ���� �� ���� ���ڿ� ȹ�� ����.");
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// ���� ���� �˸� �� ���� �̵� (Quick Match)
//////////////////////////////////////////////////////////////////////////

// ��� �����ڰ� ���� �� ȣ�� (Quick Match)
void UMultiplayerSessionsSubsystem::OnQuickMatchReady()
{
    // ���� ���� �˸� ���
    PrintString("��� �÷��̾ �𿴽��ϴ�! ���� ���� 5�� �� ���� �̵��մϴ�.");

    // 5�� �� ���� ������ �̵��ϴ� Ÿ�̸� ����
    FTimerHandle TimerHandle;
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMultiplayerSessionsSubsystem::TravelToGameLevel, 5.f, false);
    }
}

// Ÿ�̸Ӱ� ����Ǹ� ���� ������ �̵�
void UMultiplayerSessionsSubsystem::TravelToGameLevel()
{
    // ���� ���� �� ������ �� �̸��� ����� ���� ����. ���⼭�� ���÷� ������ ��� ���.
    FString GameMap = "/Game/Maps/GameMap";
    if (GetWorld())
    {
        PrintString("QuickMatch: ���� ������ �̵��մϴ�.");
        GetWorld()->ServerTravel(GameMap + "?listen");
    }
}

//////////////////////////////////////////////////////////////////////////
// Room List / Browse Game ���� ���
//////////////////////////////////////////////////////////////////////////

// Room List�� Create Game ��ư�� ������ �� ȣ�� (�κ� ���� ����)
void UMultiplayerSessionsSubsystem::CreateGameSession(FString ServerName)
{
    // TODO: ���� ���� �� �κ� ������ �̵�
    PrintString("CreateGameSession: ��� ���� �ʿ�");
}

// Room List���� ���� ���� �� Join Game ��ư�� ������ �� ȣ��
void UMultiplayerSessionsSubsystem::JoinGameSession()
{
    // TODO: ������ ���� ���� Ȯ�� �� �ο� �� üũ �� ���� ó��
    //       - ���õ� ������ ������ "���� �������ּ���" �޽��� ���
    //       - ���õ� ������ ���� á���� "������ ���� á���ϴ�" �޽��� ���
    //       - �׷��� ������ ���� ���� ����
    PrintString("JoinGameSession: ��� ���� �ʿ�");
}

//////////////////////////////////////////////////////////////////////////
// �κ񿡼� ���� ���� ���� ���
//////////////////////////////////////////////////////////////////////////

// �κ� ������ Game Start ��ư�� ������ �� ȣ��
void UMultiplayerSessionsSubsystem::StartGameFromLobby()
{
    // TODO: ���� �÷��̾� ���� �ش� ����� �䱸 �ο��� Ȯ���Ͽ�
    //       ������ ������ ���� ����, �ƴϸ� �˸� ���
    PrintString("StartGameFromLobby: ��� ���� �ʿ�");
}

//////////////////////////////////////////////////////////////////////////
// ���� ��� - �÷��̾� �߹�
//////////////////////////////////////////////////////////////////////////

// ������ ������ �÷��̾ �߹��� �� ȣ��
void UMultiplayerSessionsSubsystem::KickPlayer(const FUniqueNetIdRepl& PlayerId)
{
    // TODO: ������ �÷��̾ ���ǿ��� �߹��ϴ� ���� ����
    PrintString("KickPlayer: ��� ���� �ʿ�");
}

//////////////////////////////////////////////////////////////////////////
// ��ƿ��Ƽ �Լ�: ���� ��, ���� ���, ���� �÷��̾� �� Ȯ�� ��
//////////////////////////////////////////////////////////////////////////

FString UMultiplayerSessionsSubsystem::GetRandomMap()
{
    // TODO: ���� �� ���� ����
    return FString("RandomMap");
}

FString UMultiplayerSessionsSubsystem::GetRandomMode()
{
    // TODO: ���� ��� ���� ���� (��: "2vs2", "Solo", "AIMatch" ��)
    return FString("RandomMode");
}

int32 UMultiplayerSessionsSubsystem::GetCurrentPlayerCount()
{
    // TODO: ���� ���� �÷��̾� �� ��ȯ ���� ����
    return 0;
}
