#include "T8PlayerController.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"

AT8PlayerController::AT8PlayerController()
{
    // 기본 설정
}

void AT8PlayerController::RequestMoveToSlot(int32 SlotIndex)
{
    // 클라이언트 UI에서 호출할 함수
    // 서버 RPC 호출
    UE_LOG(LogTemp, Log, TEXT("RequestMoveToSlot: Client calling ServerRequestMoveToSlot for slot %d"), SlotIndex);
    ServerRequestMoveToSlot(SlotIndex);
}

bool AT8PlayerController::ServerRequestMoveToSlot_Validate(int32 SlotIndex)
{
    // 간단한 유효성 검사 - 음수가 아닌 슬롯 인덱스만 허용
    return SlotIndex >= 0;
}

void AT8PlayerController::ServerRequestMoveToSlot_Implementation(int32 SlotIndex)
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

void AT8PlayerController::AddAIToSlot(int32 SlotIndex)
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