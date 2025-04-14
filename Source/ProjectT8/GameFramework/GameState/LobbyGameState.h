#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Common/CustomGameState.h"
#include "LobbyGameState.generated.h"

class UUserWidget;
class APlayerState;

UENUM(BlueprintType)
enum class ETeamSetup : uint8
{
    FreeForAll UMETA(DisplayName = "Free-For-All"),
    TwoTeams   UMETA(DisplayName = "Two Teams"),
    FourTeams  UMETA(DisplayName = "Four Teams")
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

    // 추가로 SceneCapture 결과를 표시하기 위한 머티리얼/텍스처 정보도 가질 수 있습니다 (예: UTextureRenderTarget2D* 등)
    // 하지만 렌더 대상은 네트워크로 직접 복제하지 않고, 각 클라이언트에서 해당 Player/AІ의 미리보기 SceneCapture를 활용해 UI에 표시합니다.
    // （Requirement 1: 실제 플레이어는 Steam OSS로부터 닉네임・아바타를 가져와 DisplayName 및 아바타 이미지를 설정하고, 
    //                AI는 "AI Player n" 이름과 기본 아바타/모델 미리보기로 표시합니다）

    FSlotInfo()
        : PlayerState(nullptr)
        , bIsAI(false)
        , DisplayName(TEXT(""))
    {
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlotsUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeamModeChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectedMapChanged);

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
    UPROPERTY(ReplicatedUsing = OnRep_SelectedMap, BlueprintReadOnly, Category = "Lobby")
    FString SelectedMap;

    // 이벤트 delegate
    UPROPERTY(BlueprintAssignable, Category = "Lobby")
    FOnSlotsUpdated OnSlotsUpdated;
    UPROPERTY(BlueprintAssignable, Category = "Lobby")
    FOnTeamModeChanged OnTeamModeChanged;
    UPROPERTY(BlueprintAssignable, Category = "Lobby")
    FOnSelectedMapChanged OnSelectedMapChanged;

    // --- 서버측 함수 (호스트 전용) ---
    void AddAIToSlot(int32 SlotIndex);
    void RemoveSlotOccupant(int32 SlotIndex);
    void MovePlayerToSlot(APlayerState* Player, int32 NewSlotIndex);
    void SetTeamSetup(ETeamSetup NewSetup);
    void CycleMapChoice();
    void AssignPlayerToSlot(APlayerState* NewPlayer);
    void RemovePlayerFromSlot(APlayerState* LeavingPlayer);

protected:
    /** AI 플레이어 추가시 이름 생성을 위한 인덱스 (서버 전용) */
    int32 NextAIIndex;

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void OnRep_Slots();
    UFUNCTION()
    void OnRep_TeamSetup();
    UFUNCTION()
    void OnRep_SelectedMap();
};
