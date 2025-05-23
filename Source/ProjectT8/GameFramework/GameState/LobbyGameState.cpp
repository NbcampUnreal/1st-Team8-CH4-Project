﻿#include "LobbyGameState.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GameFramework/Common/T8PlayerState.h"

ALobbyGameState::ALobbyGameState()
{
    bReplicates = true;

    // 슬롯 배열을 최대 8개의 빈 슬롯으로 초기화
    Slots.SetNum(8);
    for (FSlotInfo& Slot : Slots)
    {
        Slot = FSlotInfo();
    }

    TeamSetup = ETeamSetup::FreeForAll;
}

void ALobbyGameState::BeginPlay()
{
    Super::BeginPlay();

    SetGamePhase(EGamePhase::Lobby);

    // 서버만 초기 설정
    if (HasAuthority())
    {
        TeamSetup = ETeamSetup::FreeForAll;
        UpdateTeamAssignments();
    }
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ALobbyGameState, Slots);
    DOREPLIFETIME(ALobbyGameState, TeamSetup);
}

void ALobbyGameState::OnRep_Slots()
{
    OnSlotsUpdated.Broadcast();
}

void ALobbyGameState::OnRep_TeamSetup()
{
    OnTeamModeChanged.Broadcast();
}

void ALobbyGameState::UpdateTeamAssignments()
{
    if (!HasAuthority()) return; // Server only

    UE_LOG(LogTemp, Log, TEXT("Updating team assignments for TeamSetup: %d"), static_cast<int32>(TeamSetup));

    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        FSlotInfo& Slot = Slots[i];
        int32 NewTeamNumber = -1; // Default for empty slots

        // Only assign teams to occupied slots
        if (Slot.PlayerState != nullptr || Slot.bIsAI)
        {
            switch (TeamSetup)
            {
            case ETeamSetup::FreeForAll:
                NewTeamNumber = i; // 0 to 7 for each slot
                break;
            case ETeamSetup::TwoTeams:
                NewTeamNumber = (i < 4) ? 0 : 1; // Slots 0-3 are Team 0, Slots 4-7 are Team 1
                break;
            case ETeamSetup::FourTeams:
                // Slots 0,1 = Team 0
                // Slots 2,3 = Team 1
                // Slots 4,5 = Team 2
                // Slots 6,7 = Team 3
                NewTeamNumber = i / 2;
                break;
            }
        }

        // Update the team number in the slot info
        Slot.TeamNumber = NewTeamNumber;

        // If this slot is occupied by a real player, update their PlayerState's TeamNumber
        if (Slot.PlayerState != nullptr)
        {
            AT8PlayerState* T8PlayerState = Cast<AT8PlayerState>(Slot.PlayerState);
            if (T8PlayerState)
            {
                // This change will be replicated automatically because AT8PlayerState::TeamNumber is replicated.
                T8PlayerState->TeamNumber = NewTeamNumber;
                UE_LOG(LogTemp, Verbose, TEXT("Assigned Team %d to Player %s in Slot %d"), NewTeamNumber, *T8PlayerState->GetPlayerName(), i);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("UpdateTeamAssignments: PlayerState in slot %d is not an AT8PlayerState!"), i);
            }
        }
        else if (Slot.bIsAI)
        {
            UE_LOG(LogTemp, Verbose, TEXT("Assigned Team %d to AI %s in Slot %d"), NewTeamNumber, *Slot.DisplayName, i);
        }
    }

    // Manually trigger the OnRep function on the server *if* you need immediate server-side updates
    // that depend on the OnRep logic (like updating server UI).
    // Replication to clients happens automatically due to DOREPLIFETIME.
    // OnRep_Slots(); // Usually not needed here unless server UI relies on it.

    // Since the Slots array content changed, replication will handle updating clients.
    // The OnRep_Slots function will broadcast OnSlotsUpdated on clients when they receive the update.
}

void ALobbyGameState::AddAIToSlot(int32 SlotIndex)
{
    if (!HasAuthority()) return;
    if (SlotIndex < 0 || SlotIndex >= Slots.Num()) return;

    FSlotInfo& Slot = Slots[SlotIndex];
    if (Slot.PlayerState != nullptr || Slot.bIsAI)
    {
        return;
    }

    // 사용되지 않는 가장 낮은 AI 번호를 찾기
    TArray<int32> UsedNumbers;
    
    // 먼저 현재 사용 중인 모든 AI 번호를 수집
    for (const FSlotInfo& ExistingSlot : Slots)
    {
        if (ExistingSlot.bIsAI && !ExistingSlot.DisplayName.IsEmpty())
        {
            // "AI Player X" 형식에서 숫자 부분만 추출
            FString DisplayName = ExistingSlot.DisplayName;
            int32 SpaceIndex = DisplayName.Find(TEXT(" "), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
            
            if (SpaceIndex != INDEX_NONE)
            {
                FString NumberString = DisplayName.Mid(SpaceIndex + 1);
                int32 Number = FCString::Atoi(*NumberString);
                
                if (Number > 0)
                {
                    UsedNumbers.Add(Number);
                }
            }
        }
    }
    
    // 사용 번호 정렬
    UsedNumbers.Sort();
    
    // 가장 작은 사용되지 않는 번호 찾기
    int32 AINumber = 1;  // 기본값은 1
    for (int32 UsedNumber : UsedNumbers)
    {
        if (AINumber < UsedNumber)
        {
            // 현재 AINumber가 사용되지 않았으므로 이 번호 사용
            break;
        }
        else if (AINumber == UsedNumber)
        {
            // 이미 사용중이므로 다음 번호 확인
            AINumber++;
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Adding AI with number %d to slot %d"), AINumber, SlotIndex);
    
    Slot.bIsAI = true;
    Slot.DisplayName = FString::Printf(TEXT("AI Player %d"), AINumber);
    OnSlotsUpdated.Broadcast();
}

void ALobbyGameState::RemoveSlotOccupant(int32 SlotIndex)
{
    if (!HasAuthority()) return;
    if (SlotIndex < 0 || SlotIndex >= Slots.Num()) return;

    FSlotInfo& Slot = Slots[SlotIndex];
    if (Slot.PlayerState == nullptr && !Slot.bIsAI)
    {
        return;
    }

    // 실제 플레이어인 경우, 추가 처리가 필요할 수 있으나 여기서는 슬롯 초기화 처리
    Slot.PlayerState = nullptr;
    Slot.bIsAI = false;
    Slot.DisplayName = TEXT("");
    OnSlotsUpdated.Broadcast();
}

void ALobbyGameState::MovePlayerToSlot(APlayerState* Player, int32 NewSlotIndex)
{
    UE_LOG(LogTemp, Log, TEXT("MovePlayerToSlot: Player %s to slot %d"), 
        Player ? *Player->GetPlayerName() : TEXT("NULL"), NewSlotIndex);
    
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("MovePlayerToSlot: Must be called on server"));
        return;
    }
    
    if (Player == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("MovePlayerToSlot: Player is NULL"));
        return;
    }
    
    if (NewSlotIndex < 0 || NewSlotIndex >= Slots.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("MovePlayerToSlot: Invalid slot index %d"), NewSlotIndex);
        return;
    }

    // 대상 슬롯에 이미 플레이어나 AI가 있으면 실패
    if (Slots[NewSlotIndex].PlayerState != nullptr || Slots[NewSlotIndex].bIsAI)
    {
        UE_LOG(LogTemp, Warning, TEXT("MovePlayerToSlot: Slot %d is already occupied"), NewSlotIndex);
        return;
    }

    // 플레이어가 기존에 어느 슬롯에 속해 있는지 찾기
    int32 CurrentIndex = INDEX_NONE;
    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        if (Slots[i].PlayerState == Player)
        {
            CurrentIndex = i;
            break;
        }
    }
    
    // 현재 슬롯이 없거나 이미 목표 슬롯에 있으면 처리 중단
    if (CurrentIndex == NewSlotIndex)
    {
        UE_LOG(LogTemp, Warning, TEXT("MovePlayerToSlot: Player already in slot %d"), NewSlotIndex);
        return;
    }
    
    // 현재 슬롯이 없는 경우 (새 플레이어)
    if (CurrentIndex == INDEX_NONE)
    {
        UE_LOG(LogTemp, Log, TEXT("MovePlayerToSlot: New player being assigned to slot %d"), NewSlotIndex);
        // 새로운 슬롯에 플레이어 할당
        Slots[NewSlotIndex].PlayerState = Player;
        Slots[NewSlotIndex].bIsAI = false;
        Slots[NewSlotIndex].DisplayName = Player->GetPlayerName();
        OnSlotsUpdated.Broadcast();
        return;
    }

    // 슬롯 이동 처리
    UE_LOG(LogTemp, Log, TEXT("MovePlayerToSlot: Moving player from slot %d to %d"), CurrentIndex, NewSlotIndex);
    
    // 이전 슬롯 초기화
    Slots[CurrentIndex].PlayerState = nullptr;
    Slots[CurrentIndex].bIsAI = false;
    Slots[CurrentIndex].DisplayName = TEXT("");

    // 새로운 슬롯에 플레이어 할당
    Slots[NewSlotIndex].PlayerState = Player;
    Slots[NewSlotIndex].bIsAI = false;
    Slots[NewSlotIndex].DisplayName = Player->GetPlayerName();

    // 단 한 번만 브로드캐스트
    OnSlotsUpdated.Broadcast();
}

void ALobbyGameState::SetTeamSetup(ETeamSetup NewSetup)
{
    if (!HasAuthority()) return;
    if (TeamSetup == NewSetup) return;

    UE_LOG(LogTemp, Log, TEXT("LobbyGameState: Changing team setup from %d to %d"), (int32)TeamSetup, (int32)NewSetup);

    // 팀 모드 변경
    ETeamSetup OldSetup = TeamSetup;
    TeamSetup = NewSetup;
    
    // 변경사항 브로드캐스트
    OnTeamModeChanged.Broadcast();
    
    // 주의: 슬롯 재배치는 SlotStructure 클래스에서 처리
    // 이 브로드캐스트를 통해 SlotStructure::UpdateTeamMode가 호출됨
}

void ALobbyGameState::AssignPlayerToSlot(APlayerState* NewPlayer)
{
    if (!HasAuthority() || NewPlayer == nullptr) return;

    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        if (Slots[i].PlayerState == nullptr && !Slots[i].bIsAI)
        {
            Slots[i].PlayerState = NewPlayer;
            Slots[i].bIsAI = false;
            Slots[i].DisplayName = NewPlayer->GetPlayerName();
            OnSlotsUpdated.Broadcast();
            return;
        }
    }
}

void ALobbyGameState::RemovePlayerFromSlot(APlayerState* LeavingPlayer)
{
    if (!HasAuthority() || LeavingPlayer == nullptr) return;

    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        if (Slots[i].PlayerState == LeavingPlayer)
        {
            Slots[i].PlayerState = nullptr;
            Slots[i].bIsAI = false;
            Slots[i].DisplayName = TEXT("");
            OnSlotsUpdated.Broadcast();
            break;
        }
    }
}

int32 ALobbyGameState::GetTeamNumberForAI(const FString& DisplayName) const
{
    for (const FSlotInfo& Slot : Slots)
    {
        if (Slot.bIsAI && Slot.DisplayName == DisplayName)
        {
            return Slot.TeamNumber;
        }
    }
    return -1; // 찾지 못한 경우
}
