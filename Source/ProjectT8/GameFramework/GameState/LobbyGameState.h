#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Common/CustomGameState.h"
#include "GameFramework/GameState.h"
#include "LobbyGameState.generated.h"

class UUserWidget;
class APlayerState;

UENUM(BlueprintType)
enum class ETeamSetup : uint8
{
    FreeForAll = 0 UMETA(DisplayName = "Free For All"),
    TwoTeams = 1   UMETA(DisplayName = "Two Teams"),
    FourTeams = 2  UMETA(DisplayName = "Four Teams")
};

USTRUCT(BlueprintType)
struct FSlotInfo
{
    GENERATED_BODY()

    // 슬롯에 유저(PlayerState) 존재 여부 (nullptr이면 AI 또는 빈 슬롯)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    APlayerState* PlayerState;  // 실제 플레이어인 경우 해당 PlayerState (Steam 닉네임 등에 접근 가능)

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsAI;                 // true이면 AI 슬롯, false이면 실제 플레이어 또는 빈 슬롯

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString DisplayName;        // 표시 이름: 실제 플레이어는 Steam 닉네임, AI는 "AI Player n" 등

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 TeamNumber;

    FSlotInfo()
        : PlayerState(nullptr)
        , bIsAI(false)
        , DisplayName(TEXT(""))
        , TeamNumber(-1)
    {
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlotsUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeamModeChanged);

UCLASS()
class PROJECTT8_API ALobbyGameState : public ACustomGameState
{
	GENERATED_BODY()
	
public:
    ALobbyGameState();

    UPROPERTY(ReplicatedUsing = OnRep_Slots, BlueprintReadOnly, Category = "Lobby")
    TArray<FSlotInfo> Slots;
    UPROPERTY(ReplicatedUsing = OnRep_TeamSetup, BlueprintReadOnly, Category = "Lobby")
    ETeamSetup TeamSetup;

    // 이벤트 delegate
    UPROPERTY(BlueprintAssignable, Category = "Lobby")
    FOnSlotsUpdated OnSlotsUpdated;
    UPROPERTY(BlueprintAssignable, Category = "Lobby")
    FOnTeamModeChanged OnTeamModeChanged;

    // --- 서버측 함수 (호스트 전용) ---
    void AddAIToSlot(int32 SlotIndex);
    void RemoveSlotOccupant(int32 SlotIndex);
    void MovePlayerToSlot(APlayerState* Player, int32 NewSlotIndex);
    void SetTeamSetup(ETeamSetup NewSetup);
    void AssignPlayerToSlot(APlayerState* NewPlayer);
    void RemovePlayerFromSlot(APlayerState* LeavingPlayer);
    void UpdateTeamAssignments();

    UFUNCTION(BlueprintCallable, Category = "Team")
    int32 GetTeamNumberForAI(const FString& DisplayName) const;

protected:
    /** AI 플레이어 추가시 이름 생성을 위한 인덱스 (서버 전용) */
    int32 NextAIIndex;

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void OnRep_Slots();
    UFUNCTION()
    void OnRep_TeamSetup();
};
