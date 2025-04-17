#include "SlotStructure.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Components/SlateWrapperTypes.h"
#include "UserSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "InputCoreTypes.h"
#include "GameFramework/PlayerController/LobbyPlayerController.h"
#include "Components/PanelWidget.h"

void USlotStructure::NativeConstruct()
{
    Super::NativeConstruct();

    LobbyGameStateRef = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
    
    // 바인딩: LobbyGameState의 슬롯 데이터 변경 시 RefreshSlotWidgets 호출
    if (LobbyGameStateRef)
    {
        // 기존 바인딩 제거 후 새로 바인딩
        LobbyGameStateRef->OnSlotsUpdated.RemoveDynamic(this, &USlotStructure::RefreshSlotWidgets);
        LobbyGameStateRef->OnTeamModeChanged.RemoveDynamic(this, &USlotStructure::OnTeamModeChanged);
        
        LobbyGameStateRef->OnSlotsUpdated.AddDynamic(this, &USlotStructure::RefreshSlotWidgets);
        LobbyGameStateRef->OnTeamModeChanged.AddDynamic(this, &USlotStructure::OnTeamModeChanged);
    }
    else
    {
        // For debug purposes - attempt to find the game state if it wasn't available at construct
        UE_LOG(LogTemp, Warning, TEXT("LobbyGameState not found at NativeConstruct, will attempt to find later"));
        
        // Set a timer to check again for game state
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USlotStructure::FindGameState, 0.5f, true);
    }

    // 초기 팀 모드 (필요시 LobbyGameState에서 가져오거나 기본값 FreeForAll)
    CurrentTeamMode = ETeamSetup::FreeForAll;
    if (TeamModeSwitcher)
    {
        TeamModeSwitcher->SetActiveWidgetIndex(0);
    }

    ContextMenuSlotIndex = -1;
    
    // 각 슬롯 버튼 바인딩
    BindSlotButtonEvents();
    
    // 호스트 전용 버튼 바인딩
    BindHostButtonEvents();
    
    // UI 초기화
    RefreshSlotWidgets();
}

void USlotStructure::FindGameState()
{
    if (!LobbyGameStateRef)
    {
        LobbyGameStateRef = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
        if (LobbyGameStateRef)
        {
            UE_LOG(LogTemp, Log, TEXT("LobbyGameState found after delay"));
            
            // 기존 바인딩 제거 후 새로 바인딩
            LobbyGameStateRef->OnSlotsUpdated.RemoveDynamic(this, &USlotStructure::RefreshSlotWidgets);
            LobbyGameStateRef->OnTeamModeChanged.RemoveDynamic(this, &USlotStructure::OnTeamModeChanged);
            
            LobbyGameStateRef->OnSlotsUpdated.AddDynamic(this, &USlotStructure::RefreshSlotWidgets);
            LobbyGameStateRef->OnTeamModeChanged.AddDynamic(this, &USlotStructure::OnTeamModeChanged);
            
            // Initialize with current team mode
            UpdateTeamMode(LobbyGameStateRef->TeamSetup);
            
            // Cancel the timer since we found the game state
            GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
            
            // Force a refresh
            RefreshSlotWidgets();
        }
    }
}

void USlotStructure::BindSlotButtonEvents()
{
    // FreeForAll Grid 버튼 바인딩
    FFAButtons.Empty();
    if (FreeForAllGrid)
    {
        int32 numChildren = FreeForAllGrid->GetChildrenCount();
        FFAButtons.SetNum(numChildren);
        for (int32 i = 0; i < numChildren; ++i)
        {
            if (UButton* slotButton = Cast<UButton>(FreeForAllGrid->GetChildAt(i)))
            {
                FFAButtons[i] = slotButton;
                // 기존 바인딩 제거 후 새로 바인딩
                slotButton->OnClicked.RemoveDynamic(this, &USlotStructure::OnSlotButtonClicked);
                slotButton->OnClicked.AddDynamic(this, &USlotStructure::OnSlotButtonClicked);
            }
        }
    }

    // TwoTeams Grid 버튼 바인딩
    TwoTeamsButtons.Empty();
    if (TwoTeamsGrid)
    {
        int32 numChildren = TwoTeamsGrid->GetChildrenCount();
        TwoTeamsButtons.SetNum(numChildren);
        for (int32 i = 0; i < numChildren; ++i)
        {
            if (UButton* slotButton = Cast<UButton>(TwoTeamsGrid->GetChildAt(i)))
            {
                TwoTeamsButtons[i] = slotButton;
                // 기존 바인딩 제거 후 새로 바인딩
                slotButton->OnClicked.RemoveDynamic(this, &USlotStructure::OnSlotButtonClicked);
                slotButton->OnClicked.AddDynamic(this, &USlotStructure::OnSlotButtonClicked);
            }
        }
    }

    // FourTeams Grid 버튼 바인딩
    FourTeamsButtons.Empty();
    if (FourTeamsGrid)
    {
        int32 numChildren = FourTeamsGrid->GetChildrenCount();
        FourTeamsButtons.SetNum(numChildren);
        for (int32 i = 0; i < numChildren; ++i)
        {
            if (UButton* slotButton = Cast<UButton>(FourTeamsGrid->GetChildAt(i)))
            {
                FourTeamsButtons[i] = slotButton;
                // 기존 바인딩 제거 후 새로 바인딩
                slotButton->OnClicked.RemoveDynamic(this, &USlotStructure::OnSlotButtonClicked);
                slotButton->OnClicked.AddDynamic(this, &USlotStructure::OnSlotButtonClicked);
            }
        }
    }
}

void USlotStructure::UpdateTeamMode(ETeamSetup NewTeamMode)
{
    if (CurrentTeamMode == NewTeamMode) return;
    
    UE_LOG(LogTemp, Log, TEXT("SlotStructure: Updating team mode from %d to %d"), (int32)CurrentTeamMode, (int32)NewTeamMode);
    
    // 현재 플레이어 위치 및 상태 저장
    TMap<APlayerState*, int32> PlayerPositions;
    TMap<int32, bool> SlotIsAI;
    
    if (LobbyGameStateRef)
    {
        for (int32 i = 0; i < LobbyGameStateRef->Slots.Num(); ++i)
        {
            // 실제 플레이어 위치 저장
            if (LobbyGameStateRef->Slots[i].PlayerState)
            {
                PlayerPositions.Add(LobbyGameStateRef->Slots[i].PlayerState, i);
            }
            
            // AI 위치 저장
            if (LobbyGameStateRef->Slots[i].bIsAI)
            {
                SlotIsAI.Add(i, true);
            }
        }
    }
    
    // 팀 모드 변경
    CurrentTeamMode = NewTeamMode;
    
    // 위젯 스위처 업데이트
    if (TeamModeSwitcher)
    {
        int32 targetIndex = 0;
        switch (NewTeamMode)
        {
            case ETeamSetup::FreeForAll: targetIndex = 0; break;
            case ETeamSetup::TwoTeams:   targetIndex = 1; break;
            case ETeamSetup::FourTeams:  targetIndex = 2; break;
            default:                     targetIndex = 0; break;
        }
        
        UE_LOG(LogTemp, Log, TEXT("Setting WidgetSwitcher to index: %d"), targetIndex);
        TeamModeSwitcher->SetActiveWidgetIndex(targetIndex);
    }
    
    // 1단계 서버에서만: 플레이어와 AI 재배치
    if (LobbyGameStateRef && IsLocalPlayerHost())
    {
        // 모든 슬롯을 일단 비움 (새 레이아웃에 맞게 재배치하기 위해)
        TArray<FSlotInfo> TempSlots = LobbyGameStateRef->Slots;
        for (int32 i = 0; i < LobbyGameStateRef->Slots.Num(); ++i)
        {
            LobbyGameStateRef->Slots[i].PlayerState = nullptr;
            LobbyGameStateRef->Slots[i].bIsAI = false;
            LobbyGameStateRef->Slots[i].DisplayName = TEXT("");
        }
        
        // 플레이어 재배치
        for (const auto& Pair : PlayerPositions)
        {
            if (Pair.Key)
            {
                // 새 팀 모드에 맞는 슬롯 인덱스 계산
                int32 NewSlotIndex = DetermineNewSlotIndex(Pair.Value, NewTeamMode);
                
                // 슬롯이 이미 사용 중인지 확인
                while (LobbyGameStateRef->Slots[NewSlotIndex].PlayerState != nullptr || 
                      LobbyGameStateRef->Slots[NewSlotIndex].bIsAI)
                {
                    // 다음 가능한 슬롯으로 이동
                    NewSlotIndex = (NewSlotIndex + 1) % LobbyGameStateRef->Slots.Num();
                }
                
                // 플레이어 데이터 복사
                LobbyGameStateRef->Slots[NewSlotIndex].PlayerState = Pair.Key;
                LobbyGameStateRef->Slots[NewSlotIndex].bIsAI = false;
                
                // 원래 슬롯에서 표시 이름 가져오기
                for (int32 i = 0; i < TempSlots.Num(); ++i)
                {
                    if (TempSlots[i].PlayerState == Pair.Key)
                    {
                        LobbyGameStateRef->Slots[NewSlotIndex].DisplayName = TempSlots[i].DisplayName;
                        break;
                    }
                }
                
                if (LobbyGameStateRef->Slots[NewSlotIndex].DisplayName.IsEmpty())
                {
                    // 표시 이름을 가져오지 못한 경우 기본값 설정
                    LobbyGameStateRef->Slots[NewSlotIndex].DisplayName = Pair.Key->GetPlayerName();
                }
                
                UE_LOG(LogTemp, Log, TEXT("Player moved from slot %d to %d"), Pair.Value, NewSlotIndex);
            }
        }
        
        // AI 재배치
        for (const auto& Pair : SlotIsAI)
        {
            int32 OldSlotIndex = Pair.Key;
            int32 NewSlotIndex = DetermineNewSlotIndex(OldSlotIndex, NewTeamMode);
            
            // 슬롯이 이미 사용 중인지 확인
            while (LobbyGameStateRef->Slots[NewSlotIndex].PlayerState != nullptr || 
                  LobbyGameStateRef->Slots[NewSlotIndex].bIsAI)
            {
                // 다음 가능한 슬롯으로 이동
                NewSlotIndex = (NewSlotIndex + 1) % LobbyGameStateRef->Slots.Num();
            }
            
            // AI 데이터 복사
            LobbyGameStateRef->Slots[NewSlotIndex].PlayerState = nullptr;
            LobbyGameStateRef->Slots[NewSlotIndex].bIsAI = true;
            
            // 원래 슬롯에서 표시 이름 가져오기
            for (int32 i = 0; i < TempSlots.Num(); ++i)
            {
                if (i == OldSlotIndex && TempSlots[i].bIsAI)
                {
                    LobbyGameStateRef->Slots[NewSlotIndex].DisplayName = TempSlots[i].DisplayName;
                    break;
                }
            }
            
            if (LobbyGameStateRef->Slots[NewSlotIndex].DisplayName.IsEmpty())
            {
                // 표시 이름을 가져오지 못한 경우 기본값 설정
                LobbyGameStateRef->Slots[NewSlotIndex].DisplayName = FString::Printf(TEXT("AI Player %d"), NewSlotIndex + 1);
            }
            
            UE_LOG(LogTemp, Log, TEXT("AI moved from slot %d to %d"), OldSlotIndex, NewSlotIndex);
        }
        
        // 슬롯 업데이트 브로드캐스트
        LobbyGameStateRef->OnSlotsUpdated.Broadcast();
    }
    
    // UI 새로고침
    RefreshSlotWidgets();
}

int32 USlotStructure::DetermineNewSlotIndex(int32 OldIndex, ETeamSetup NewMode)
{
    int32 totalSlots = (LobbyGameStateRef) ? LobbyGameStateRef->Slots.Num() : 8;
    
    // 팀에 따라 인덱스 매핑
    switch (NewMode)
    {
        case ETeamSetup::FreeForAll:
            // FFA에서는 단순 배치
            return OldIndex % totalSlots;
            
        case ETeamSetup::TwoTeams:
        {
            // 현재 팀 계산 (FFA 또는 4팀에서 2팀으로 변경 시)
            int32 currentTeam;
            
            if (CurrentTeamMode == ETeamSetup::FreeForAll)
            {
                // FFA에서는 교대로 팀 할당 (0,2,4,6 -> 팀1, 1,3,5,7 -> 팀2)
                currentTeam = OldIndex % 2;
            }
            else if (CurrentTeamMode == ETeamSetup::FourTeams)
            {
                // 4팀에서는 두 팀씩 합침 (팀1,2 -> 새 팀1, 팀3,4 -> 새 팀2)
                currentTeam = (OldIndex >= totalSlots/2) ? 1 : 0;
            }
            else
            {
                // 이미 2팀인 경우
                return OldIndex;
            }
            
            // 팀 내에서의 포지션
            int32 posInTeam = (OldIndex / 2) % (totalSlots / 2);
            
            // 새 인덱스 계산
            return currentTeam * (totalSlots / 2) + posInTeam;
        }
            
        case ETeamSetup::FourTeams:
        {
            // 현재 팀 계산 (FFA 또는 2팀에서 4팀으로 변경 시)
            int32 currentTeam;
            
            if (CurrentTeamMode == ETeamSetup::FreeForAll)
            {
                // FFA에서는 순서대로 팀 할당 (0,1 -> 팀1, 2,3 -> 팀2, 4,5 -> 팀3, 6,7 -> 팀4)
                currentTeam = (OldIndex / 2) % 4;
            }
            else if (CurrentTeamMode == ETeamSetup::TwoTeams)
            {
                // 2팀에서는 각 팀을 둘로 나눔
                currentTeam = (OldIndex >= totalSlots/2) ? 2 + (OldIndex % 2) : (OldIndex % 2);
            }
            else
            {
                // 이미 4팀인 경우
                return OldIndex;
            }
            
            // 팀 내에서의 포지션
            int32 posInTeam = (OldIndex / 4) % (totalSlots / 4);
            
            // 새 인덱스 계산
            return currentTeam * (totalSlots / 4) + posInTeam;
        }
            
        default:
            return OldIndex;
    }
}

void USlotStructure::RefreshSlotWidgets()
{
    if (!LobbyGameStateRef)
    {
        LobbyGameStateRef = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
        if (!LobbyGameStateRef)
        {
            UE_LOG(LogTemp, Warning, TEXT("RefreshSlotWidgets: LobbyGameState not found"));
            return;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("RefreshSlotWidgets: Updating UI with current slots"));

    TArray<FSlotInfo>& slotsData = LobbyGameStateRef->Slots;
    int32 totalSlots = slotsData.Num();
    TMap<APlayerState*, UUserWidget*> newSlotWidgetMap;
    newSlotWidgetMap.Reserve(totalSlots);

    TArray<UButton*>* ActiveButtons = GetActiveButtonArray();
    if (!ActiveButtons)
    {
        UE_LOG(LogTemp, Error, TEXT("RefreshSlotWidgets: No active buttons available!"));
        return;
    }

    // Get local player controller to check if user is host
    bool bIsHost = IsLocalPlayerHost();

    // 현재 활성화된 그리드 인덱스 (0, 1, 2)
    int32 activeGridIndex = 0;
    switch (CurrentTeamMode)
    {
        case ETeamSetup::FreeForAll: activeGridIndex = 0; break;
        case ETeamSetup::TwoTeams:   activeGridIndex = 1; break;
        case ETeamSetup::FourTeams:  activeGridIndex = 2; break;
        default:                     activeGridIndex = 0; break;
    }

    for (int32 i = 0; i < totalSlots && i < ActiveButtons->Num(); ++i)
    {
        UButton* targetButton = (*ActiveButtons)[i];
        if (!targetButton) continue;
        
        // Get slot data
        FSlotInfo SlotData = slotsData.IsValidIndex(i) ? slotsData[i] : FSlotInfo();
        APlayerState* playerState = SlotData.PlayerState;
        bool bIsAI = SlotData.bIsAI;
        
        // Handle occupied slots (Players or AI)
        if ((playerState || bIsAI) && targetButton)
        {
            UUserWidget* userSlotWidget = nullptr;
            
            // Reuse existing widget if possible
            if (playerState && SlotWidgetMap.Contains(playerState))
            {
                userSlotWidget = SlotWidgetMap[playerState];
                if (userSlotWidget && userSlotWidget->GetParent() != targetButton)
                {
                    userSlotWidget->RemoveFromParent();
                    targetButton->SetContent(userSlotWidget);
                }
            }
            else
            {
                // Create new widget
                if (UserSlotWidgetClass)
                {
                    userSlotWidget = CreateWidget<UUserWidget>(GetWorld(), UserSlotWidgetClass);
                    if (userSlotWidget)
                    {
                        // Configure the user slot (player name, AI status, etc.)
                        UUserSlot* SlotWidget = Cast<UUserSlot>(userSlotWidget);
                        if (SlotWidget)
                        {
                            SlotWidget->SetupSlot(SlotData.DisplayName, playerState, bIsAI, i, bIsHost);
                        }
                        
                        targetButton->SetContent(userSlotWidget);
                    }
                }
            }
            
            if (userSlotWidget)
            {
                if (playerState)
            {
                newSlotWidgetMap.Add(playerState, userSlotWidget);
                }
                
                // Disable the button for occupied slots
                targetButton->SetIsEnabled(false);
            }
        }
        else
        {
            // Clear empty slots
            if (UWidget* existingWidget = targetButton->GetContent())
            {
                existingWidget->RemoveFromParent();
            }
            
            // Enable the button for empty slots
            targetButton->SetIsEnabled(true);
            
            // 중요: RefreshSlotWidgets에서는 버튼 이벤트를 바인딩하지 않음
            // 모든 버튼 바인딩은 BindSlotButtonEvents()에서만 처리
        }
        
        // 호스트 전용 버튼 상태 업데이트
        const int32 MaxSlotsPerGrid = 8;
        for (int32 j = 0; j < MaxSlotsPerGrid; ++j)
        {
            // 각 그리드별 배열 인덱스 계산
            int32 arrayIndex = activeGridIndex * MaxSlotsPerGrid + j;
            
            // 배열 범위 확인
            if (arrayIndex < AddAIButtons.Num() && arrayIndex < KickButtons.Num())
            {
                UButton* addAIButton = AddAIButtons[arrayIndex];
                UButton* kickButton = KickButtons[arrayIndex];
                
                if (addAIButton && kickButton)
                {
                    // 호스트가 아니면 버튼 숨김
                    if (!bIsHost)
                    {
                        addAIButton->SetVisibility(ESlateVisibility::Hidden);
                        kickButton->SetVisibility(ESlateVisibility::Hidden);
                        continue;
                    }
                    
                    // 호스트 본인 슬롯이면 버튼 비활성화
                    if (IsHostSlot(j))
                    {
                        addAIButton->SetVisibility(ESlateVisibility::Hidden);
                        kickButton->SetVisibility(ESlateVisibility::Hidden);
                        continue;
                    }
                    
                    // 슬롯 데이터 가져오기 - 변수 이름 변경하여 충돌 방지
                    FSlotInfo SlotDataForButton = slotsData.IsValidIndex(j) ? slotsData[j] : FSlotInfo();
                    APlayerState* playerStateForButton = SlotDataForButton.PlayerState;
                    bool bIsAIForButton = SlotDataForButton.bIsAI;
                    
                    // 호스트가 아닌 사용자 슬롯이면
                    if (playerStateForButton && !IsHostSlot(j))
                    {
                        // AI 추가 버튼 숨김 (이미 플레이어가 있음)
                        addAIButton->SetVisibility(ESlateVisibility::Hidden);
                        
                        // 추방 버튼 표시
                        kickButton->SetVisibility(ESlateVisibility::Visible);
                        kickButton->SetToolTipText(FText::FromString(TEXT("플레이어 추방")));
                    }
                    // AI 슬롯이면
                    else if (bIsAIForButton)
                    {
                        // AI 추가 버튼 숨김 (이미 AI가 있음)
                        addAIButton->SetVisibility(ESlateVisibility::Hidden);
                        
                        // 추방 버튼 표시
                        kickButton->SetVisibility(ESlateVisibility::Visible);
                        kickButton->SetToolTipText(FText::FromString(TEXT("AI 제거")));
                    }
                    // 빈 슬롯이면
                    else
                    {
                        // AI 추가 버튼 표시
                        addAIButton->SetVisibility(ESlateVisibility::Visible);
                        addAIButton->SetToolTipText(FText::FromString(TEXT("AI 추가")));
                        
                        // 추방 버튼 숨김 (제거할 대상 없음)
                        kickButton->SetVisibility(ESlateVisibility::Hidden);
                    }
                }
            }
        }
    }

    // Clean up old widgets that are no longer used
    for (const auto& kv : SlotWidgetMap)
    {
        APlayerState* oldPlayer = kv.Key;
        UUserWidget* oldWidget = kv.Value;
        if (oldWidget && !newSlotWidgetMap.Contains(oldPlayer))
        {
            oldWidget->RemoveFromParent();
        }
    }
    
    // Update widget map
    SlotWidgetMap = MoveTemp(newSlotWidgetMap);
}

void USlotStructure::OnSlotButtonClicked()
{
    // 가끔 중복 호출되는 것을 방지하기 위한 타이머 확인
    static double LastClickTime = 0.0;
    double CurrentTime = FPlatformTime::Seconds();
    if (CurrentTime - LastClickTime < 0.5)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnSlotButtonClicked: Ignoring too frequent click"));
        return;
    }
    LastClickTime = CurrentTime;

    int32 clickedIndex = -1;
    TArray<UButton*>* ActiveButtons = nullptr;
    switch (CurrentTeamMode)
    {
        case ETeamSetup::FreeForAll: ActiveButtons = &FFAButtons; break;
        case ETeamSetup::TwoTeams:   ActiveButtons = &TwoTeamsButtons; break;
        case ETeamSetup::FourTeams:  ActiveButtons = &FourTeamsButtons; break;
        default:                     ActiveButtons = &FFAButtons; break;
    }
    if (!ActiveButtons) return;

    for (int32 i = 0; i < ActiveButtons->Num(); ++i)
    {
        UButton* btn = (*ActiveButtons)[i];
        if (btn && (btn->HasUserFocus(GetOwningPlayer()) || btn->IsHovered()))
        {
            clickedIndex = i;
            break;
        }
    }
    if (clickedIndex < 0) return;

    UE_LOG(LogTemp, Log, TEXT("Slot button clicked: %d"), clickedIndex);

    // 프로그래머가 검증할 수 있도록 모든 로그 출력
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;
    
    // Shift 키 확인
    bool bIsShiftDown = PC->IsInputKeyDown(EKeys::LeftShift) || PC->IsInputKeyDown(EKeys::RightShift);
    UE_LOG(LogTemp, Log, TEXT("Shift key pressed: %s"), bIsShiftDown ? TEXT("True") : TEXT("False"));

    // 슬롯이 비어있는지 확인 (플레이어 또는 AI가 없는 경우)
    bool bIsEmpty = false;
    
    if (LobbyGameStateRef && clickedIndex < LobbyGameStateRef->Slots.Num())
    {
        bIsEmpty = !LobbyGameStateRef->Slots[clickedIndex].PlayerState && !LobbyGameStateRef->Slots[clickedIndex].bIsAI;
    }
    
    // 플레이어가 이미 해당 슬롯에 있는지 확인
    bool bPlayerAlreadyInSlot = false;
    if (LobbyGameStateRef && PC->PlayerState)
    {
        bPlayerAlreadyInSlot = (LobbyGameStateRef->Slots[clickedIndex].PlayerState == PC->PlayerState);
        if (bPlayerAlreadyInSlot)
        {
            UE_LOG(LogTemp, Warning, TEXT("OnSlotButtonClicked: Player already in this slot"));
            return;
        }
    }

    // LobbyPlayerController로 캐스팅
    ALobbyPlayerController* LobbyPC = Cast<ALobbyPlayerController>(PC);
    if (!LobbyPC)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast to LobbyPlayerController"));
        return;
    }

    // AI 추가 또는 슬롯 변경 요청
    if (bIsShiftDown && IsLocalPlayerHost() && bIsEmpty)
    {
        UE_LOG(LogTemp, Log, TEXT("Adding AI to slot %d"), clickedIndex);
        LobbyPC->AddAIToSlot(clickedIndex);
    }
    else if (bIsEmpty)
    {
        UE_LOG(LogTemp, Log, TEXT("Requesting move to slot %d"), clickedIndex);
        LobbyPC->RequestMoveToSlot(clickedIndex);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Slot %d is already occupied"), clickedIndex);
    }
}

void USlotStructure::ShowAddAIOption(int32 SlotIndex)
{
    if (!IsLocalPlayerHost()) return;
    
    ContextMenuSlotIndex = SlotIndex;
    
    // Shift+클릭으로 AI 추가하는 방법을 표시
    UE_LOG(LogTemp, Log, TEXT("To add AI to slot %d, hold Shift and click on the slot"), SlotIndex);
}

void USlotStructure::AddAIToSlot(int32 SlotIndex)
{
    if (!IsLocalPlayerHost()) return;
    
    UE_LOG(LogTemp, Log, TEXT("Adding AI to slot %d"), SlotIndex);
    
    if (LobbyGameStateRef)
    {
        // Verify slot is empty before adding AI
        if (SlotIndex >= 0 && SlotIndex < LobbyGameStateRef->Slots.Num())
        {
            if (!LobbyGameStateRef->Slots[SlotIndex].PlayerState && !LobbyGameStateRef->Slots[SlotIndex].bIsAI)
            {
                LobbyGameStateRef->AddAIToSlot(SlotIndex);
                UE_LOG(LogTemp, Log, TEXT("AI added to slot %d"), SlotIndex);
            }
        }
    }
}

bool USlotStructure::IsLocalPlayerHost() const
{
    APlayerController* LocalPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    return LocalPC && LocalPC->HasAuthority();
}

// OnTeamModeChanged 델리게이트 핸들러 - ETeamSetup 파라미터 없이 동작
void USlotStructure::OnTeamModeChanged()
{
    if (LobbyGameStateRef)
    {
        // 현재 설정된 팀 모드로 UI 업데이트
        UpdateTeamMode(LobbyGameStateRef->TeamSetup);
    }
}

// 헬퍼 함수 구현
bool USlotStructure::IsHostSlot(int32 SlotIndex) const
{
    if (!LobbyGameStateRef || SlotIndex < 0 || SlotIndex >= LobbyGameStateRef->Slots.Num())
    {
        return false;
    }
    
    APlayerController* PC = GetOwningPlayer();
    if (!PC || !PC->PlayerState)
    {
        return false;
    }
    
    return LobbyGameStateRef->Slots[SlotIndex].PlayerState == PC->PlayerState;
}

TArray<UButton*>* USlotStructure::GetActiveButtonArray()
{
    switch (CurrentTeamMode)
    {
        case ETeamSetup::FreeForAll: return &FFAButtons;
        case ETeamSetup::TwoTeams:   return &TwoTeamsButtons;
        case ETeamSetup::FourTeams:  return &FourTeamsButtons;
        default:                     return &FFAButtons;
    }
}

bool USlotStructure::IsSlotEmpty(int32 SlotIndex) const
{
    if (!LobbyGameStateRef || SlotIndex < 0 || SlotIndex >= LobbyGameStateRef->Slots.Num())
    {
        return false;
    }
    
    return !LobbyGameStateRef->Slots[SlotIndex].PlayerState && !LobbyGameStateRef->Slots[SlotIndex].bIsAI;
}

bool USlotStructure::IsSlotAI(int32 SlotIndex) const
{
    if (!LobbyGameStateRef || SlotIndex < 0 || SlotIndex >= LobbyGameStateRef->Slots.Num())
    {
        return false;
    }
    
    return LobbyGameStateRef->Slots[SlotIndex].bIsAI;
}

bool USlotStructure::IsSlotPlayer(int32 SlotIndex) const
{
    if (!LobbyGameStateRef || SlotIndex < 0 || SlotIndex >= LobbyGameStateRef->Slots.Num())
    {
        return false;
    }
    
    return LobbyGameStateRef->Slots[SlotIndex].PlayerState != nullptr && !LobbyGameStateRef->Slots[SlotIndex].bIsAI;
}

// 버튼에 해당하는 슬롯 인덱스를 찾는 함수 추가
int32 USlotStructure::GetSlotIndexForButton(UButton* Button) const
{
    if (!Button) return -1;
    
    const int32* FoundIndex = ButtonToSlotMap.Find(Button);
    if (FoundIndex)
    {
        return *FoundIndex;
    }
    
    return -1;
}

// 호스트 전용 버튼 바인딩 함수
void USlotStructure::BindHostButtonEvents()
{
    // 먼저 버튼의 가시성 설정 (호스트만 볼 수 있음)
    bool bIsHost = IsLocalPlayerHost();
    
    // 맵 초기화
    ButtonToSlotMap.Empty();
    LastClickedButton = nullptr;
    
    // AddAIButtons와 KickButtons 배열 초기화 (최대 8개 * 3그리드 = 24개)
    const int32 MaxSlotsPerGrid = 8;
    const int32 TotalMaxSlots = MaxSlotsPerGrid * 3;
    
    if (AddAIButtons.Num() < TotalMaxSlots)
    {
        AddAIButtons.SetNum(TotalMaxSlots);
    }
    
    if (KickButtons.Num() < TotalMaxSlots)
    {
        KickButtons.SetNum(TotalMaxSlots);
    }
    
    // 그리드 별 버튼 바인딩 (3개 그리드, 각 8개씩)
    // 버튼 이름 규칙: 
    // - 첫 번째 그리드(FFA): AddAI_11 ~ AddAI_18, Kick_11 ~ Kick_18
    // - 두 번째 그리드(TwoTeams): AddAI_21 ~ AddAI_28, Kick_21 ~ Kick_28
    // - 세 번째 그리드(FourTeams): AddAI_31 ~ AddAI_38, Kick_31 ~ Kick_38
    
    for (int32 gridIndex = 0; gridIndex < 3; ++gridIndex)
    {
        int32 gridPrefix = gridIndex + 1; // 1, 2, 3
        
        for (int32 slotIndex = 0; slotIndex < MaxSlotsPerGrid; ++slotIndex)
        {
            // 0~7을 1~8로 변환
            int32 buttonNumber = slotIndex + 1;
            
            // 버튼 이름 생성 (예: "AddAI_11", "AddAI_21", "AddAI_31" 등)
            FString addAIButtonName = FString::Printf(TEXT("AddAI_%d%d"), gridPrefix, buttonNumber);
            FString kickButtonName = FString::Printf(TEXT("Kick_%d%d"), gridPrefix, buttonNumber);
            
            // 배열 인덱스 계산 (0~23)
            int32 arrayIndex = gridIndex * MaxSlotsPerGrid + slotIndex;
            
            // AI 추가 버튼 찾기
            UButton* addAIButton = Cast<UButton>(GetWidgetFromName(FName(*addAIButtonName)));
            if (addAIButton)
            {
                // 버튼 가시성 설정
                addAIButton->SetVisibility(bIsHost ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
                
                // 기존 바인딩 제거
                addAIButton->OnClicked.Clear();
                
                // 버튼과 슬롯 인덱스 매핑 (각 그리드 내에서의 슬롯 인덱스 0~7만 저장)
                ButtonToSlotMap.Add(addAIButton, slotIndex);
                AddAIButtons[arrayIndex] = addAIButton;
                
                // 버튼 클릭 이벤트 바인딩
                addAIButton->OnClicked.AddDynamic(this, &USlotStructure::OnAddAIButtonClicked);
            }
            
            // 추방 버튼 찾기
            UButton* kickButton = Cast<UButton>(GetWidgetFromName(FName(*kickButtonName)));
            if (kickButton)
            {
                // 버튼 가시성 설정
                kickButton->SetVisibility(bIsHost ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
                
                // 기존 바인딩 제거
                kickButton->OnClicked.Clear();
                
                // 버튼과 슬롯 인덱스 매핑 (각 그리드 내에서의 슬롯 인덱스 0~7만 저장)
                ButtonToSlotMap.Add(kickButton, slotIndex);
                KickButtons[arrayIndex] = kickButton;
                
                // 버튼 클릭 이벤트 바인딩
                kickButton->OnClicked.AddDynamic(this, &USlotStructure::OnKickButtonClicked);
            }
        }
    }
}

// AI 추가 버튼 클릭 이벤트 핸들러
void USlotStructure::OnAddAIButtonClicked()
{
    // 호스트인지 확인
    if (!IsLocalPlayerHost())
    {
        return;
    }
    
    // 클릭된 버튼 찾기
    UButton* ClickedButton = nullptr;
    
    // 현재 활성화된 그리드 인덱스에 해당하는 버튼들만 검사
    int32 activeGridIndex = 0;
    switch (CurrentTeamMode)
    {
        case ETeamSetup::FreeForAll: activeGridIndex = 0; break;
        case ETeamSetup::TwoTeams:   activeGridIndex = 1; break;
        case ETeamSetup::FourTeams:  activeGridIndex = 2; break;
        default:                     activeGridIndex = 0; break;
    }
    
    const int32 MaxSlotsPerGrid = 8;
    int32 startIndex = activeGridIndex * MaxSlotsPerGrid;
    int32 endIndex = startIndex + MaxSlotsPerGrid;
    
    // 해당 그리드의 버튼들만 검사
    for (int32 i = startIndex; i < endIndex && i < AddAIButtons.Num(); ++i)
    {
        UButton* Button = AddAIButtons[i];
        if (Button && (Button->HasUserFocus(GetOwningPlayer()) || Button->IsHovered()))
        {
            ClickedButton = Button;
            break;
        }
    }
    
    if (!ClickedButton)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnAddAIButtonClicked: Couldn't determine which button was clicked"));
        return;
    }
    
    // 버튼에 매핑된 슬롯 인덱스 찾기
    int32 SlotIndex = GetSlotIndexForButton(ClickedButton);
    if (SlotIndex == -1)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnAddAIButtonClicked: Invalid slot index"));
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("OnAddAIButtonClicked: Slot %d"), SlotIndex);
    
    // 슬롯이 비어있는지 확인
    if (IsSlotEmpty(SlotIndex))
    {
        AddAIToSlot(SlotIndex);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnAddAIButtonClicked: Slot %d is not empty"), SlotIndex);
    }
}

// 추방 버튼 클릭 이벤트 핸들러
void USlotStructure::OnKickButtonClicked()
{
    // 호스트인지 확인
    if (!IsLocalPlayerHost())
    {
        return;
    }
    
    // 클릭된 버튼 찾기
    UButton* ClickedButton = nullptr;
    
    // 현재 활성화된 그리드 인덱스에 해당하는 버튼들만 검사
    int32 activeGridIndex = 0;
    switch (CurrentTeamMode)
    {
        case ETeamSetup::FreeForAll: activeGridIndex = 0; break;
        case ETeamSetup::TwoTeams:   activeGridIndex = 1; break;
        case ETeamSetup::FourTeams:  activeGridIndex = 2; break;
        default:                     activeGridIndex = 0; break;
    }
    
    const int32 MaxSlotsPerGrid = 8;
    int32 startIndex = activeGridIndex * MaxSlotsPerGrid;
    int32 endIndex = startIndex + MaxSlotsPerGrid;
    
    // 해당 그리드의 버튼들만 검사
    for (int32 i = startIndex; i < endIndex && i < KickButtons.Num(); ++i)
    {
        UButton* Button = KickButtons[i];
        if (Button && (Button->HasUserFocus(GetOwningPlayer()) || Button->IsHovered()))
        {
            ClickedButton = Button;
            break;
        }
    }
    
    if (!ClickedButton)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnKickButtonClicked: Couldn't determine which button was clicked"));
        return;
    }
    
    // 버튼에 매핑된 슬롯 인덱스 찾기
    int32 SlotIndex = GetSlotIndexForButton(ClickedButton);
    if (SlotIndex == -1)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnKickButtonClicked: Invalid slot index"));
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("OnKickButtonClicked: Slot %d"), SlotIndex);
    
    // 자신의 슬롯인지 확인
    if (IsHostSlot(SlotIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("OnKickButtonClicked: Cannot kick yourself"));
        return;
    }
    
    // 슬롯이 비어있는지 확인
    if (IsSlotEmpty(SlotIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("OnKickButtonClicked: Slot %d is empty"), SlotIndex);
        return;
    }
    
    // AI인 경우에만 제거, 플레이어인 경우 아무 동작도 하지 않음
    if (IsSlotAI(SlotIndex))
    {
        RemoveAIFromSlot(SlotIndex);
    }
    // 플레이어인 경우 아무 동작도 하지 않음
    else if (IsSlotPlayer(SlotIndex))
    {
		KickPlayerFromSlot(SlotIndex);
    }
}

void USlotStructure::RemoveAIFromSlot(int32 SlotIndex)
{
    if (!IsLocalPlayerHost()) return;
    
    UE_LOG(LogTemp, Log, TEXT("Removing AI from slot %d"), SlotIndex);
    
    // AI가 있는지 확인
    if (!IsSlotAI(SlotIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("RemoveAIFromSlot: No AI in slot %d"), SlotIndex);
        return;
    }
    
    // LobbyPlayerController로 캐스팅
    ALobbyPlayerController* LobbyPC = Cast<ALobbyPlayerController>(GetOwningPlayer());
    if (!LobbyPC)
    {
        UE_LOG(LogTemp, Error, TEXT("RemoveAIFromSlot: Failed to cast to LobbyPlayerController"));
        return;
    }
    
    // 컨트롤러에 요청 전달
    LobbyPC->RemoveAIFromSlot(SlotIndex);
}

void USlotStructure::KickPlayerFromSlot(int32 SlotIndex)
{
    /*if (IsLocalPlayerHost())
    {
		UE_LOG(LogTemp, Log, TEXT("Host can't be kicked"));
        return;
    }*/
    if (!IsSlotPlayer(SlotIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("KickPlayerFromSlot: No player in slot %d"), SlotIndex);
        return;
    }

    UWorld* World = GetWorld();
    ALobbyGameState* LobbyGS = World->GetGameState<ALobbyGameState>();
    if (!LobbyGS)
    {
        UE_LOG(LogTemp, Error, TEXT("KickPlayerFromSlot: Failed to get LobbyGameState"));
        return;
    }

    // 플레이어 컨트롤러 가져오기
    APlayerController* PlayerToKick = nullptr;
    if (LobbyGS->Slots[SlotIndex].PlayerState)
    {
        PlayerToKick = LobbyGS->Slots[SlotIndex].PlayerState->GetPlayerController();
    }

    if (!PlayerToKick)
    {
        UE_LOG(LogTemp, Error, TEXT("KickPlayerFromSlot: Failed to get PlayerController for slot %d"), SlotIndex);
        return;
    }

    // LobbyPlayerController로 캐스팅
    ALobbyPlayerController* LobbyKickPC = Cast<ALobbyPlayerController>(PlayerToKick);
    if (!LobbyKickPC)
    {
        UE_LOG(LogTemp, Error, TEXT("KickPlayerFromSlot: Failed to cast to LobbyPlayerController"));
        return;
    }

    // 우리 자신의 컨트롤러를 가져와서 서버 권한 확인 (이미 IsLocalPlayerHost에서 확인했지만 이중 확인)
    ALobbyPlayerController* OurPC = Cast<ALobbyPlayerController>(GetOwningPlayer());
    if (!OurPC || !OurPC->HasAuthority())
    {
        UE_LOG(LogTemp, Error, TEXT("KickPlayerFromSlot: Not authorized to kick players"));
        return;
    }

    // 1. 먼저 슬롯에서 플레이어 정보 제거
    LobbyGS->RemovePlayerFromSlot(LobbyGS->Slots[SlotIndex].PlayerState);

    // 2. 해당 플레이어에게 PrivateLevel로 이동하도록 알림
    LobbyKickPC->ClientTravelToPrivateLevel();

    UE_LOG(LogTemp, Log, TEXT("KickPlayerFromSlot: Player kicked from slot %d"), SlotIndex);
}