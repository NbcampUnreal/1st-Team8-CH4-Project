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

    // 기존 자식들을 클리어하고 버튼 배열 초기화
    SlotGridPanel->ClearChildren();
    SlotButtons.Empty();

    const int32 NumSlots = 8;
    // 예시: 2행 x 4열 배열 (필요에 따라 변경 가능)
    for (int32 Index = 0; Index < NumSlots; ++Index)
    {
        // UButton 인스턴스 생성 (Owner는 this)
        UButton* NewButton = NewObject<UButton>(this, UButton::StaticClass());
        if (NewButton)
        {
            int32 Row = Index / 2;
            int32 Column = Index % 2;
            // 그리드 패널에 버튼을 추가 (AddChildToUniformGrid()가 UUniformGridSlot* 반환)
            UUniformGridSlot* GridSlot = SlotGridPanel->AddChildToUniformGrid(NewButton, Row, Column);
            SlotButtons.Add(NewButton);

            // 각 버튼마다 개별적으로 OnClicked 이벤트에 UFUNCTION을 바인딩합니다.
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

    // LobbyGameState의 MovePlayerToSlot()를 호출하여 슬롯 이동 요청 실행
    LobbyState->MovePlayerToSlot(PC->PlayerState, SlotIndex);
}

// 개별 슬롯 버튼 클릭 핸들러 - 각 함수는 공통 함수에 해당 인덱스를 전달합니다.
void USlotStructure::OnSlotButton0Clicked() { OnSlotButtonClicked(0); }
void USlotStructure::OnSlotButton1Clicked() { OnSlotButtonClicked(1); }
void USlotStructure::OnSlotButton2Clicked() { OnSlotButtonClicked(2); }
void USlotStructure::OnSlotButton3Clicked() { OnSlotButtonClicked(3); }
void USlotStructure::OnSlotButton4Clicked() { OnSlotButtonClicked(4); }
void USlotStructure::OnSlotButton5Clicked() { OnSlotButtonClicked(5); }
void USlotStructure::OnSlotButton6Clicked() { OnSlotButtonClicked(6); }
void USlotStructure::OnSlotButton7Clicked() { OnSlotButtonClicked(7); }
