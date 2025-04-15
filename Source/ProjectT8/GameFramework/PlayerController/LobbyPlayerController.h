#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 커스텀 PlayerController 클래스
 * 멀티플레이어 기능 및 RPC를 구현
 */
UCLASS()
class PROJECTT8_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    ALobbyPlayerController();

    // 클라이언트에서 서버로 슬롯 이동 요청
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRequestMoveToSlot(int32 SlotIndex);
    
    // 슬롯 관련 기능
    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void RequestMoveToSlot(int32 SlotIndex);
    
    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void AddAIToSlot(int32 SlotIndex);
    
    // AI 제거 기능
    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void RemoveAIFromSlot(int32 SlotIndex);
    
    // 플레이어 추방 기능
    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void KickPlayerFromSlot(int32 SlotIndex);
}; 