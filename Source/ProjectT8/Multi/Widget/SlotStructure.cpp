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
    
    BindSlotButtonEvents();
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

    TArray<UButton*>* ActiveButtons = nullptr;
    switch (CurrentTeamMode)
    {
        case ETeamSetup::FreeForAll: ActiveButtons = &FFAButtons; break;
        case ETeamSetup::TwoTeams:   ActiveButtons = &TwoTeamsButtons; break;
        case ETeamSetup::FourTeams:  ActiveButtons = &FourTeamsButtons; break;
        default:                     ActiveButtons = &FFAButtons; break;
    }
    if (!ActiveButtons)
    {
        UE_LOG(LogTemp, Error, TEXT("RefreshSlotWidgets: No active buttons available!"));
        return;
    }

    // Get local player controller to check if user is host
    bool bIsHost = IsLocalPlayerHost();

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

void USlotStructure::ServerRequestMoveToSlot(int32 SlotIndex)
{
    // 이 함수는 이제 사용하지 않습니다 - 모든 요청은 T8PlayerController를 통해 처리
    UE_LOG(LogTemp, Warning, TEXT("ServerRequestMoveToSlot is deprecated. Use T8PlayerController::RequestMoveToSlot instead"));
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