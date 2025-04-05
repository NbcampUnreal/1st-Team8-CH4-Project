#include "Multi/MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/Engine.h"
#include "TimerManager.h" // 타이머 관련

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
    // TODO: 현재 Quick Match 세션 검색 후,
    //       있으면 해당 세션에 참가, 없으면 랜덤 맵/모드로 새 세션 생성
    PrintString("QuickMatch: 기능 구현 필요");
}

// 세션 검색 완료 delegate (Quick Match용)
void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
    // TODO: 검색 결과 처리 후,
    //       Quick Match 세션이 있으면 Join, 없으면 CreateQuickMatchSession 호출
    PrintString("OnFindSessionsComplete: 기능 구현 필요");
}

// 새로운 Quick Match 세션 생성
void UMultiplayerSessionsSubsystem::CreateQuickMatchSession()
{
    // TODO: 랜덤 맵과 랜덤 모드를 선택하여 Quick Match 세션 생성
    PrintString("CreateQuickMatchSession: 기능 구현 필요");
}

// 세션 생성 완료 delegate (Quick Match 또는 다른 세션용)
void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    // TODO: 세션 생성 성공 여부에 따른 후속 처리
    // 예를 들어, Quick Match의 경우 필요한 인원이 모이면 OnQuickMatchReady 호출
    PrintString("OnCreateSessionComplete: 기능 구현 필요");
    
    // 예시: Quick Match 세션에서 모든 플레이어가 모였다고 가정
    if (SessionName == FName("QuickMatchSession") && bWasSuccessful)
    {
        // TODO: 실제 플레이어 수 체크 후 조건 만족 시 호출하도록 수정
        OnQuickMatchReady();
    }
}

// 세션 참가 완료 delegate
void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    // TODO: 참가 완료 후 클라이언트 이동 처리(ClientTravel 등)
    PrintString("OnJoinSessionComplete: 기능 구현 필요");
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
    // TODO: 실제 게임 레벨 이름(예: 랜덤으로 선택된 맵)을 적용
    if (GetWorld())
    {
        GetWorld()->ServerTravel("/Game/Maps/GameMap?listen");
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
