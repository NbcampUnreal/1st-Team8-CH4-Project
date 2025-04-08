#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSessionSettings.h"           // FOnlineSessionSettings 사용
#include "Interfaces/OnlineSessionInterface.h" // IOnlineSessionPtr, FOnlineSessionSearchResult 등 사용
#include "MultiplayerSessionsSubsystem.generated.h"

//======================================================================
// Delegate Declarations
//======================================================================

// 서버(세션) 생성 완료 시 브로드캐스트 (성공 여부)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerCreateDelegate, bool, WasSuccessful);
// 서버(세션) 참가 시도 완료 시 브로드캐스트 (성공 여부)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerJoinDelegate, bool, WasSuccessful);
// 서버(세션) 파괴 완료 시 브로드캐스트 (성공 여부) - 호스트용
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerDestroyDelegate, bool, WasSuccessful);
// 클라이언트 세션 나가기 완료 시 브로드캐스트 (성공 여부) - 클라이언트용
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientLeaveSessionDelegate, bool, bWasSuccessful);

//======================================================================
// Struct Declarations
//======================================================================

USTRUCT(BlueprintType)
struct FGameModeData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Mode")
	FString ModeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Mode")
	int32 RequiredPlayers;

	// 기본 생성자
	FGameModeData()
		: ModeName(TEXT("")), RequiredPlayers(0)
	{
	}

	// 인자 있는 생성자
	FGameModeData(FString InModeName, int32 InRequiredPlayers)
		: ModeName(InModeName), RequiredPlayers(InRequiredPlayers)
	{
	}
};

//======================================================================
// Class Definition
//======================================================================

UCLASS()
class PROJECTT8_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//======================================================================
	// Constructor & Overrides
	//======================================================================
	UMultiplayerSessionsSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//======================================================================
	// Public Functions - Blueprint Callable (세션 관리)
	//======================================================================

	/**
	 * @brief 지정된 이름으로 새로운 게임 세션(로비)을 생성합니다. (호스트 전용)
	 * @param ServerName 세션 목록에 표시될 서버(방) 이름.
	 */
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Sessions | Host")
	void CreateGameSession(FString ServerName);

	/**
	 * @brief 지정된 이름의 서버(세션)를 검색합니다.
	 * @param ServerName 검색할 서버(방) 이름.
	 */
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Sessions | Client")
	void FindSession(FString ServerName);

	/**
	 * @brief 현재 참가 중인 세션을 파괴합니다. (호스트 전용)
	 */
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Sessions | Host")
	void DestroySession();

	/**
	 * @brief 현재 참가 중인 세션에서 나갑니다. (클라이언트 전용)
	 */
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Sessions | Client")
	void LeaveSession();

	//======================================================================
	// Public Functions - Blueprint Callable (게임 관리)
	//======================================================================

	/**
	 * @brief 로비에서 게임 시작을 시도합니다. (호스트 전용, TODO: 구현 필요)
	 */
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Sessions | Host")
	void StartGameFromLobby();

	/**
	 * @brief 지정된 플레이어를 세션에서 추방합니다. (호스트 전용, TODO: 구현 필요)
	 * @param PlayerId 추방할 플레이어의 UniqueNetId.
	 */
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Sessions | Host")
	void KickPlayer(const FUniqueNetIdRepl& PlayerId);

	//======================================================================
	// Public Functions - Utilities (C++ 전용)
	//======================================================================

	/**
	 * @brief 지정된 경로에서 랜덤 맵 경로를 가져옵니다.
	 * @return 랜덤 맵의 에셋 경로.
	 */
	FString GetRandomMap();

	/**
	 * @brief 미리 정의된 게임 모드 데이터 중 하나를 랜덤으로 가져옵니다.
	 * @return 랜덤 게임 모드 데이터.
	 */
	FGameModeData GetRandomMode();

	/**
	 * @brief 현재 세션의 플레이어 수를 반환합니다. (Quick Match 용도로 보임, TODO: 일반 세션용으로 수정 필요)
	 * @return 현재 등록된 플레이어 수.
	 */
	int32 GetCurrentPlayerCount();

	//======================================================================
	// Public Delegates - Blueprint Assignable
	//======================================================================

	// 세션 생성 완료 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Sessions | Delegates")
	FServerCreateDelegate ServerCreateDel;

	// 세션 참가 완료 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Sessions | Delegates")
	FServerJoinDelegate ServerJoinDel;

	// 세션 파괴 완료 델리게이트 (호스트)
	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Sessions | Delegates")
	FServerDestroyDelegate ServerDestroyDel;

	// 세션 나가기 완료 델리게이트 (클라이언트)
	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Sessions | Delegates")
	FClientLeaveSessionDelegate ClientLeaveSessionDel;


protected:
	//======================================================================
	// Protected Functions - Online Subsystem Callbacks
	//======================================================================
	// 이 함수들은 Online Subsystem에 의해 내부적으로 호출됩니다. 직접 호출하지 마세요.

	/** @brief 세션 생성 완료 시 호출되는 콜백 */
	void OnCreateSessionComplete(FName SessionName, bool WasSuccessful);

	/** @brief 세션 검색 완료 시 호출되는 콜백 */
	void OnFindSessionsComplete(bool WasSuccessful);

	/** @brief 세션 참가 완료 시 호출되는 콜백 */
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/** @brief 세션 파괴 또는 나가기 완료 시 호출되는 콜백 */
	void OnDestroySessionComplete(FName SessionName, bool WasSuccessful);

private:
	//======================================================================
	// Private Member Variables - Session Management
	//======================================================================

	// Unreal Engine Online Session Interface 포인터
	IOnlineSessionPtr SessionInterface;

	// 세션 검색 결과를 저장하기 위한 공유 포인터
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	// 서브시스템이 관리하는 주 세션의 내부 이름 (보통 NAME_GameSession)
	FName MySessionName = FName(NAME_GameSession);

	// 현재 사용자가 세션의 호스트인지 여부 (Destroy/Leave 구분용)
	bool bIsHost = false;

	// 파괴 후 재생성 관련 상태 변수 (CreateGameSession 로직용)
	bool CreateServerAfterDestroy = false;
	FString DestroyServerName = TEXT("");

	// 현재 검색 중인 서버 이름 (FindSession <-> OnFindSessionsComplete 간 상태 저장용)
	FString ServerNameToFind = TEXT("");

	//======================================================================
	// Private Member Variables & Functions - Quick Match (주석 처리됨)
	//======================================================================
	/*
	// Quick Match 세션 생성 시 선택한 랜덤 맵 경로 (나중에 레벨 이동 시 사용)
	FString QuickMatchSelectedMap;
	// Quick Match 세션의 요구 플레이어 수
	int32 QuickMatchRequiredPlayers;
	// Quick Match 플레이어 수 폴링 타이머 핸들
	FTimerHandle QuickMatchPollTimerHandle;

	// Quick Match 플레이어 수를 주기적으로 확인하는 함수
	void PollQuickMatchPlayerCount();

	// Quick Match 관련 함수들 (주석 처리됨)
	void QuickMatch();
	void OnFindQuickSessionsComplete(bool bWasSuccessful);
	void CreateQuickMatchSession();
	void OnCreateQuickSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnJoinQuickSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnQuickMatchReady();
	void TravelToGameLevel();
	*/
};