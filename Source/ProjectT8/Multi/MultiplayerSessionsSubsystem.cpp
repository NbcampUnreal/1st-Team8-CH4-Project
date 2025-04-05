#include "Multi/MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/Engine.h"
#include "TimerManager.h" // Ÿ�̸� ����

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
    // TODO: ���� Quick Match ���� �˻� ��,
    //       ������ �ش� ���ǿ� ����, ������ ���� ��/���� �� ���� ����
    PrintString("QuickMatch: ��� ���� �ʿ�");
}

// ���� �˻� �Ϸ� delegate (Quick Match��)
void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
    // TODO: �˻� ��� ó�� ��,
    //       Quick Match ������ ������ Join, ������ CreateQuickMatchSession ȣ��
    PrintString("OnFindSessionsComplete: ��� ���� �ʿ�");
}

// ���ο� Quick Match ���� ����
void UMultiplayerSessionsSubsystem::CreateQuickMatchSession()
{
    // TODO: ���� �ʰ� ���� ��带 �����Ͽ� Quick Match ���� ����
    PrintString("CreateQuickMatchSession: ��� ���� �ʿ�");
}

// ���� ���� �Ϸ� delegate (Quick Match �Ǵ� �ٸ� ���ǿ�)
void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    // TODO: ���� ���� ���� ���ο� ���� �ļ� ó��
    // ���� ���, Quick Match�� ��� �ʿ��� �ο��� ���̸� OnQuickMatchReady ȣ��
    PrintString("OnCreateSessionComplete: ��� ���� �ʿ�");
    
    // ����: Quick Match ���ǿ��� ��� �÷��̾ �𿴴ٰ� ����
    if (SessionName == FName("QuickMatchSession") && bWasSuccessful)
    {
        // TODO: ���� �÷��̾� �� üũ �� ���� ���� �� ȣ���ϵ��� ����
        OnQuickMatchReady();
    }
}

// ���� ���� �Ϸ� delegate
void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    // TODO: ���� �Ϸ� �� Ŭ���̾�Ʈ �̵� ó��(ClientTravel ��)
    PrintString("OnJoinSessionComplete: ��� ���� �ʿ�");
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
    // TODO: ���� ���� ���� �̸�(��: �������� ���õ� ��)�� ����
    if (GetWorld())
    {
        GetWorld()->ServerTravel("/Game/Maps/GameMap?listen");
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
