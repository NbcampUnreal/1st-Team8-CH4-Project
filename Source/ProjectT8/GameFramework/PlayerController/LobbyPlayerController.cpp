#include "LobbyPlayerController.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

ALobbyPlayerController::ALobbyPlayerController()
{
    // 기본 설정
}

// --- 슬롯 이동 관련 함수 ---
void ALobbyPlayerController::RequestMoveToSlot(int32 SlotIndex)
{
    UE_LOG(LogTemp, Log, TEXT("RequestMoveToSlot: Client calling ServerRequestMoveToSlot for slot %d"), SlotIndex);
    ServerRequestMoveToSlot(SlotIndex);
}

bool ALobbyPlayerController::ServerRequestMoveToSlot_Validate(int32 SlotIndex)
{
    return SlotIndex >= 0;
}

void ALobbyPlayerController::ServerRequestMoveToSlot_Implementation(int32 SlotIndex)
{
    UE_LOG(LogTemp, Log, TEXT("ServerRequestMoveToSlot: Running on server, slot %d"), SlotIndex);

    ALobbyGameState* LobbyState = Cast<ALobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
    if (!LobbyState)
    {
        UE_LOG(LogTemp, Warning, TEXT("ServerRequestMoveToSlot: LobbyGameState not found"));
        return;
    }

    if (SlotIndex >= LobbyState->Slots.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("ServerRequestMoveToSlot: Slot index out of range"));
        return;
    }

    if (LobbyState->Slots[SlotIndex].PlayerState != nullptr || LobbyState->Slots[SlotIndex].bIsAI)
    {
        UE_LOG(LogTemp, Warning, TEXT("ServerRequestMoveToSlot: Slot %d is already occupied"), SlotIndex);
        return;
    }

    if (PlayerState)
    {
        LobbyState->MovePlayerToSlot(PlayerState, SlotIndex);
        UE_LOG(LogTemp, Log, TEXT("ServerRequestMoveToSlot: Player moved to slot %d"), SlotIndex);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ServerRequestMoveToSlot: No PlayerState"));
    }
}

// --- AI 추가/제거 관련 함수 ---
void ALobbyPlayerController::AddAIToSlot(int32 SlotIndex)
{
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("AddAIToSlot: Must be called on server"));
        return;
    }

    ALobbyGameState* LobbyState = Cast<ALobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
    if (LobbyState)
    {
        LobbyState->AddAIToSlot(SlotIndex);
        UE_LOG(LogTemp, Log, TEXT("AddAIToSlot: AI added to slot %d"), SlotIndex);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AddAIToSlot: LobbyGameState not found"));
    }
}

void ALobbyPlayerController::RemoveAIFromSlot(int32 SlotIndex)
{
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("RemoveAIFromSlot: Must be called on server"));
        return;
    }

    ALobbyGameState* LobbyState = Cast<ALobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
    if (!LobbyState)
    {
        UE_LOG(LogTemp, Warning, TEXT("RemoveAIFromSlot: LobbyGameState not found"));
        return;
    }

    if (SlotIndex < 0 || SlotIndex >= LobbyState->Slots.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("RemoveAIFromSlot: Invalid slot index %d"), SlotIndex);
        return;
    }

    if (!LobbyState->Slots[SlotIndex].bIsAI)
    {
        UE_LOG(LogTemp, Warning, TEXT("RemoveAIFromSlot: No AI in slot %d"), SlotIndex);
        return;
    }

    LobbyState->RemoveSlotOccupant(SlotIndex);
    UE_LOG(LogTemp, Log, TEXT("RemoveAIFromSlot: AI removed from slot %d"), SlotIndex);
}

void ALobbyPlayerController::ClientTravelToPrivateLevel_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("Client kicked from lobby, returning to PrivateLevel"));

    // 클라이언트 측에서 PrivateLevel로 이동
    UGameplayStatics::OpenLevel(this, FName("PrivateLevel"));
}

void ALobbyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetInputMode(FInputModeGameAndUI());
    bShowMouseCursor = true;

    if (IsLocalController())
    {
        if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                Subsystem->AddMappingContext(LobbyMappingContext, 0);
            }
        }
    }
}

void ALobbyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EInput->BindAction(NextPhaseAction, ETriggerEvent::Triggered, this, &ALobbyPlayerController::HandleNextPhaseInput);
    }
}

void ALobbyPlayerController::HandleNextPhaseInput()
{
    UGameplayStatics::OpenLevel(this, FName("LoadingMap"));
}