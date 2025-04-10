#include "LobbyGameState.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

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
    SelectedMap = TEXT("DefaultMap");

    NextAIIndex = 1;
}

void ALobbyGameState::BeginPlay()
{
    Super::BeginPlay();

    // 서버만 초기 설정
    if (HasAuthority())
    {
        TeamSetup = ETeamSetup::FreeForAll;
        SelectedMap = TEXT("Map1");
    }
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ALobbyGameState, Slots);
    DOREPLIFETIME(ALobbyGameState, TeamSetup);
    DOREPLIFETIME(ALobbyGameState, SelectedMap);
}

void ALobbyGameState::OnRep_Slots()
{
    OnSlotsUpdated.Broadcast();
}

void ALobbyGameState::OnRep_TeamSetup()
{
    OnTeamModeChanged.Broadcast();
}

void ALobbyGameState::OnRep_SelectedMap()
{
    OnSelectedMapChanged.Broadcast();
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

    Slot.bIsAI = true;
    Slot.DisplayName = FString::Printf(TEXT("AI Player %d"), NextAIIndex++);
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
    if (!HasAuthority() || Player == nullptr) return;
    if (NewSlotIndex < 0 || NewSlotIndex >= Slots.Num()) return;

    // 대상 슬롯에 이미 플레이어나 AI가 있으면 실패
    if (Slots[NewSlotIndex].PlayerState != nullptr || Slots[NewSlotIndex].bIsAI)
    {
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
    if (CurrentIndex == INDEX_NONE || CurrentIndex == NewSlotIndex)
    {
        return;
    }

    // 이전 슬롯 초기화
    Slots[CurrentIndex].PlayerState = nullptr;
    Slots[CurrentIndex].bIsAI = false;
    Slots[CurrentIndex].DisplayName = TEXT("");

    // 새로운 슬롯에 플레이어 할당
    Slots[NewSlotIndex].PlayerState = Player;
    Slots[NewSlotIndex].bIsAI = false;
    Slots[NewSlotIndex].DisplayName = Player->GetPlayerName();

    OnSlotsUpdated.Broadcast();
}

void ALobbyGameState::SetTeamSetup(ETeamSetup NewSetup)
{
    if (!HasAuthority()) return;
    if (TeamSetup == NewSetup) return;

    TeamSetup = NewSetup;
    OnTeamModeChanged.Broadcast();
}

void ALobbyGameState::CycleMapChoice()
{
    if (!HasAuthority()) return;

    // 예시: 사용할 수 있는 맵 목록
    static const TArray<FString> MapList = { TEXT("Map1"), TEXT("Map2"), TEXT("Map3") };
    int32 CurrentIndex = MapList.IndexOfByKey(SelectedMap);
    if (CurrentIndex == INDEX_NONE)
    {
        CurrentIndex = 0;
    }
    int32 NextIndex = (CurrentIndex + 1) % MapList.Num();
    SelectedMap = MapList[NextIndex];
    OnSelectedMapChanged.Broadcast();
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
            return;
        }
    }
}
