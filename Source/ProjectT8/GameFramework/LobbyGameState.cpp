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

    // ���� �迭�� �ִ� 8���� �� �������� �ʱ�ȭ
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

    // ������ �ʱ� ����
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

    // ���� �÷��̾��� ���, �߰� ó���� �ʿ��� �� ������ ���⼭�� ���� �ʱ�ȭ ó��
    Slot.PlayerState = nullptr;
    Slot.bIsAI = false;
    Slot.DisplayName = TEXT("");
    OnSlotsUpdated.Broadcast();
}

void ALobbyGameState::MovePlayerToSlot(APlayerState* Player, int32 NewSlotIndex)
{
    if (!HasAuthority() || Player == nullptr) return;
    if (NewSlotIndex < 0 || NewSlotIndex >= Slots.Num()) return;

    // ��� ���Կ� �̹� �÷��̾ AI�� ������ ����
    if (Slots[NewSlotIndex].PlayerState != nullptr || Slots[NewSlotIndex].bIsAI)
    {
        return;
    }

    // �÷��̾ ������ ��� ���Կ� ���� �ִ��� ã��
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

    // ���� ���� �ʱ�ȭ
    Slots[CurrentIndex].PlayerState = nullptr;
    Slots[CurrentIndex].bIsAI = false;
    Slots[CurrentIndex].DisplayName = TEXT("");

    // ���ο� ���Կ� �÷��̾� �Ҵ�
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

    // ����: ����� �� �ִ� �� ���
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
