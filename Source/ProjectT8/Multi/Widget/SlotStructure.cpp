#include "SlotStructure.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Button.h"
#include "GameFramework/LobbyGameState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void USlotStructure::NativeConstruct()
{
    Super::NativeConstruct();

    SetupSlotButtons();
}

void USlotStructure::SetupSlotButtons()
{
    if (!SlotGridPanel)
    {
        UE_LOG(LogTemp, Warning, TEXT("SlotGridPanel is not bound in SlotStructure widget"));
        return;
    }

    // ���� �ڽĵ��� Ŭ�����ϰ� ��ư �迭 �ʱ�ȭ
    SlotGridPanel->ClearChildren();
    SlotButtons.Empty();

    const int32 NumSlots = 8;
    // ����: 2�� x 4�� �迭 (�ʿ信 ���� ���� ����)
    for (int32 Index = 0; Index < NumSlots; ++Index)
    {
        // UButton �ν��Ͻ� ���� (Owner�� this)
        UButton* NewButton = NewObject<UButton>(this, UButton::StaticClass());
        if (NewButton)
        {
            int32 Row = Index / 2;
            int32 Column = Index % 2;
            // �׸��� �гο� ��ư�� �߰� (AddChildToUniformGrid()�� UUniformGridSlot* ��ȯ)
            UUniformGridSlot* GridSlot = SlotGridPanel->AddChildToUniformGrid(NewButton, Row, Column);
            SlotButtons.Add(NewButton);

            // �� ��ư���� ���������� OnClicked �̺�Ʈ�� UFUNCTION�� ���ε��մϴ�.
            switch (Index)
            {
            case 0: NewButton->OnClicked.AddDynamic(this, &USlotStructure::OnSlotButton0Clicked); break;
            case 1: NewButton->OnClicked.AddDynamic(this, &USlotStructure::OnSlotButton1Clicked); break;
            case 2: NewButton->OnClicked.AddDynamic(this, &USlotStructure::OnSlotButton2Clicked); break;
            case 3: NewButton->OnClicked.AddDynamic(this, &USlotStructure::OnSlotButton3Clicked); break;
            case 4: NewButton->OnClicked.AddDynamic(this, &USlotStructure::OnSlotButton4Clicked); break;
            case 5: NewButton->OnClicked.AddDynamic(this, &USlotStructure::OnSlotButton5Clicked); break;
            case 6: NewButton->OnClicked.AddDynamic(this, &USlotStructure::OnSlotButton6Clicked); break;
            case 7: NewButton->OnClicked.AddDynamic(this, &USlotStructure::OnSlotButton7Clicked); break;
            default: break;
            }
        }
    }
}

ALobbyGameState* USlotStructure::GetLobbyGameState() const
{
    return GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
}

void USlotStructure::OnSlotButtonClicked(int32 SlotIndex)
{
    ALobbyGameState* LobbyState = GetLobbyGameState();
    if (!LobbyState)
    {
        UE_LOG(LogTemp, Warning, TEXT("LobbyGameState not found in SlotStructure"));
        return;
    }

    APlayerController* PC = GetOwningPlayer();
    if (!PC || !PC->PlayerState)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerController or PlayerState not found in SlotStructure"));
        return;
    }

    // LobbyGameState�� MovePlayerToSlot()�� ȣ���Ͽ� ���� �̵� ��û ����
    LobbyState->MovePlayerToSlot(PC->PlayerState, SlotIndex);
}

// ���� ���� ��ư Ŭ�� �ڵ鷯 - �� �Լ��� ���� �Լ��� �ش� �ε����� �����մϴ�.
void USlotStructure::OnSlotButton0Clicked() { OnSlotButtonClicked(0); }
void USlotStructure::OnSlotButton1Clicked() { OnSlotButtonClicked(1); }
void USlotStructure::OnSlotButton2Clicked() { OnSlotButtonClicked(2); }
void USlotStructure::OnSlotButton3Clicked() { OnSlotButtonClicked(3); }
void USlotStructure::OnSlotButton4Clicked() { OnSlotButtonClicked(4); }
void USlotStructure::OnSlotButton5Clicked() { OnSlotButtonClicked(5); }
void USlotStructure::OnSlotButton6Clicked() { OnSlotButtonClicked(6); }
void USlotStructure::OnSlotButton7Clicked() { OnSlotButtonClicked(7); }
