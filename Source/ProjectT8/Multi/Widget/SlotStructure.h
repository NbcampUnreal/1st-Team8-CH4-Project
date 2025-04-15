#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "SlotStructure.generated.h"

// Forward declarations
class ALobbyGameState;
class APlayerState;
class UButton;
class UPanelWidget;
class UWidgetSwitcher;
class UGridPanel;
class UUniformGridPanel;
class ALobbyPlayerController;

UCLASS()
class PROJECTT8_API USlotStructure : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Slot")
    void RefreshSlotWidgets();

    UFUNCTION(BlueprintCallable, Category = "Slot")
    void UpdateTeamMode(ETeamSetup NewTeamMode);
    
    UFUNCTION()
    void FindGameState();
    
    // Adds an AI to the specified slot
    UFUNCTION(BlueprintCallable, Category = "Slot")
    void AddAIToSlot(int32 SlotIndex);
    
    // Context menu function to add AI to slot
    UFUNCTION(BlueprintCallable, Category = "Slot")
    void ShowAddAIOption(int32 SlotIndex);

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnSlotButtonClicked();
    
    // AI 추가 버튼 클릭 이벤트 - 매개변수 없음
    UFUNCTION()
    void OnAddAIButtonClicked();
    
    // AI 제거 버튼 클릭 이벤트 - 매개변수 없음
    UFUNCTION()
    void OnKickButtonClicked();
    
    UFUNCTION()
    void OnTeamModeChanged();
    
    /** Maps slots between different team mode layouts */
    int32 DetermineNewSlotIndex(int32 OldIndex, ETeamSetup NewMode);
    
    // 클라이언트가 서버에 슬롯 이동 요청
    void ServerRequestMoveToSlot(int32 SlotIndex);

private:
    /** LobbyGameState 레퍼런스 (실시간 데이터 접근) */
    UPROPERTY()
    ALobbyGameState* LobbyGameStateRef;

    /**
     * 슬롯 위젯 클래스 (예: UUserSlot BP 클래스). BP에서 오버라이드 가능할 수 있습니다.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slot", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> UserSlotWidgetClass;

    /**
     * WidgetSwitcher: 3가지 그리드 패널을 관리합니다.
     * 인덱스 0: FreeForAllGrid, 1: TwoTeamsGrid, 2: FourTeamsGrid.
     */
    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* TeamModeSwitcher;

    /** Free-for-all 모드 그리드 패널 (GridPanel, BP에서 구현될) */
    UPROPERTY(meta = (BindWidgetOptional))
    UGridPanel* FreeForAllGrid;
    /** Two Teams 모드 그리드 패널 (UniformGridPanel, BP에서 구현될) */
    UPROPERTY(meta = (BindWidgetOptional))
    UGridPanel* TwoTeamsGrid;
    /** Four Teams 모드 그리드 패널 (UniformGridPanel, BP에서 구현될) */
    UPROPERTY(meta = (BindWidgetOptional))
    UGridPanel* FourTeamsGrid;

    /** 각 그리드 패널(모드)별 슬롯 버튼 배열 */
    TArray<UButton*> FFAButtons;
    TArray<UButton*> TwoTeamsButtons;
    TArray<UButton*> FourTeamsButtons;
    
    /** 각 슬롯에 대응되는 AI 추가 버튼 (호스트만 볼 수 있음) */
    UPROPERTY(meta = (BindWidgetOptional))
    TArray<UButton*> AddAIButtons;
    
    /** 각 슬롯에 대응되는 AI 제거 버튼 (호스트만 볼 수 있음) */
    UPROPERTY(meta = (BindWidgetOptional))
    TArray<UButton*> KickButtons;
    
    /** 버튼과 해당 슬롯 인덱스 맵핑 */
    UPROPERTY()
    TMap<UButton*, int32> ButtonToSlotMap;
    
    /** 마지막으로 클릭된 버튼 */
    UPROPERTY()
    UButton* LastClickedButton;

    /** 현재 활성화된 팀 모드 */
    ETeamSetup CurrentTeamMode;

    /** 각 플레이어에 연결된 플레이어 PlayerState와 위젯 매핑 */
    TMap<APlayerState*, UUserWidget*> SlotWidgetMap;
    
    /** Stores the slot index for AI context menu */
    int32 ContextMenuSlotIndex;

    /** 각 슬롯 버튼에 클릭 이벤트를 바인딩합니다. */
    void BindSlotButtonEvents();
    
    /** 호스트 전용 AI 추가/추방 버튼 이벤트 바인딩 */
    void BindHostButtonEvents();
    
    /** AI를 제거합니다 */
    UFUNCTION(BlueprintCallable, Category = "Slot")
    void RemoveAIFromSlot(int32 SlotIndex);
    
    /** Checks if the local player is the host */
    bool IsLocalPlayerHost() const;
    
    /** 호스트가 자신의 슬롯인지 확인합니다 */
    bool IsHostSlot(int32 SlotIndex) const;
    
    /** 현재 활성화된 버튼 배열을 가져옵니다 */
    TArray<UButton*>* GetActiveButtonArray();
    
    /** 슬롯이 비어있는지 확인합니다 */
    bool IsSlotEmpty(int32 SlotIndex) const;
    
    /** 슬롯에 AI가 있는지 확인합니다 */
    bool IsSlotAI(int32 SlotIndex) const;
    
    /** 슬롯에 플레이어가 있는지 확인합니다 */
    bool IsSlotPlayer(int32 SlotIndex) const;
    
    /** 버튼에 해당하는 슬롯 인덱스를 찾습니다 */
    int32 GetSlotIndexForButton(UButton* Button) const;
};