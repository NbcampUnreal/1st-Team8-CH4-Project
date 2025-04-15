#include "LobbyPlayerController.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"

ALobbyPlayerController::ALobbyPlayerController()
{
    // 기본 설정
}

void ALobbyPlayerController::RequestMoveToSlot(int32 SlotIndex)
{
    // 클라이언트 UI에서 호출할 함수
    // 서버 RPC 호출
    UE_LOG(LogTemp, Log, TEXT("RequestMoveToSlot: Client calling ServerRequestMoveToSlot for slot %d"), SlotIndex);
    ServerRequestMoveToSlot(SlotIndex);
}

bool ALobbyPlayerController::ServerRequestMoveToSlot_Validate(int32 SlotIndex)
{
    // 간단한 유효성 검사 - 음수가 아닌 슬롯 인덱스만 허용
    return SlotIndex >= 0;
}

void ALobbyPlayerController::ServerRequestMoveToSlot_Implementation(int32 SlotIndex)
{
    // 서버에서 실행되는 RPC 함수
    UE_LOG(LogTemp, Log, TEXT("ServerRequestMoveToSlot: Running on server, slot %d"), SlotIndex);
    
    // 로비 게임 스테이트 가져오기
    ALobbyGameState* LobbyState = Cast<ALobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
    if (!LobbyState)
    {
        UE_LOG(LogTemp, Warning, TEXT("ServerRequestMoveToSlot: LobbyGameState not found"));
        return;
    }
    
    // 슬롯이 유효한지 확인
    if (SlotIndex >= LobbyState->Slots.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("ServerRequestMoveToSlot: Slot index out of range"));
        return;
    }
    
    // 슬롯이 비어있는지 확인
    if (LobbyState->Slots[SlotIndex].PlayerState != nullptr || LobbyState->Slots[SlotIndex].bIsAI)
    {
        UE_LOG(LogTemp, Warning, TEXT("ServerRequestMoveToSlot: Slot %d is already occupied"), SlotIndex);
        return;
    }
    
    // 플레이어를 새 슬롯으로 이동
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

void ALobbyPlayerController::AddAIToSlot(int32 SlotIndex)
{
    // AI 추가 기능
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("AddAIToSlot: Must be called on server"));
        return;
    }
    
    // 서버에서는 직접 추가 가능
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
    // AI 제거 기능
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("RemoveAIFromSlot: Must be called on server"));
        return;
    }
    
    // 서버에서만 직접 제거 가능
    ALobbyGameState* LobbyState = Cast<ALobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
    if (!LobbyState)
    {
        UE_LOG(LogTemp, Warning, TEXT("RemoveAIFromSlot: LobbyGameState not found"));
        return;
    }
    
    // 슬롯 인덱스 유효성 검사
    if (SlotIndex < 0 || SlotIndex >= LobbyState->Slots.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("RemoveAIFromSlot: Invalid slot index %d"), SlotIndex);
        return;
    }
    
    // 슬롯에 AI가 있는지 확인
    if (!LobbyState->Slots[SlotIndex].bIsAI)
    {
        UE_LOG(LogTemp, Warning, TEXT("RemoveAIFromSlot: No AI in slot %d"), SlotIndex);
        return;
    }
    
    // AI 제거
    LobbyState->RemoveSlotOccupant(SlotIndex);
    UE_LOG(LogTemp, Log, TEXT("RemoveAIFromSlot: AI removed from slot %d"), SlotIndex);
}

void ALobbyPlayerController::KickPlayerFromSlot(int32 SlotIndex)
{
    // 플레이어 추방 기능
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("KickPlayerFromSlot: Must be called on server"));
        return;
    }
    
    // 서버에서만 직접 추방 가능
    ALobbyGameState* LobbyState = Cast<ALobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
    if (!LobbyState)
    {
        UE_LOG(LogTemp, Warning, TEXT("KickPlayerFromSlot: LobbyGameState not found"));
        return;
    }
    
    // 슬롯 인덱스 유효성 검사
    if (SlotIndex < 0 || SlotIndex >= LobbyState->Slots.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("KickPlayerFromSlot: Invalid slot index %d"), SlotIndex);
        return;
    }
    
    // 슬롯에 플레이어가 있는지 확인
    APlayerState* TargetPlayer = LobbyState->Slots[SlotIndex].PlayerState;
    if (!TargetPlayer)
    {
        UE_LOG(LogTemp, Warning, TEXT("KickPlayerFromSlot: No player in slot %d"), SlotIndex);
        return;
    }
    
    // 자기 자신은 추방할 수 없음
    if (TargetPlayer == PlayerState)
    {
        UE_LOG(LogTemp, Warning, TEXT("KickPlayerFromSlot: Cannot kick yourself"));
        return;
    }
    
    // 플레이어 추방
    LobbyState->RemovePlayerFromSlot(TargetPlayer);
    
    // 클라이언트에게 메인 메뉴로 돌아가도록 알림
    if (APlayerController* TargetPC = Cast<APlayerController>(TargetPlayer->GetOwner()))
    {
        TargetPC->ClientReturnToMainMenuWithTextReason(FText::FromString(TEXT("You have been kicked from the session.")));
        UE_LOG(LogTemp, Log, TEXT("KickPlayerFromSlot: Player %s kicked from slot %d"), *TargetPlayer->GetPlayerName(), SlotIndex);
    }
} 