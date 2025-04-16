#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * Lobby에서 플레이어 상호작용을 처리하는 PlayerController
 */

class UInputMappingContext;
class UInputAction;

UCLASS()
class PROJECTT8_API ALobbyPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ALobbyPlayerController();

    // 클라이언트가 서버에 슬롯 이동 요청
    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void RequestMoveToSlot(int32 SlotIndex);

    // 서버 RPC: 슬롯 이동 요청 처리
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRequestMoveToSlot(int32 SlotIndex);
    
    // 슬롯 관련 기능
    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void RequestMoveToSlot(int32 SlotIndex);
    
    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void AddAIToSlot(int32 SlotIndex);

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // Input Mapping Context
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* LobbyMappingContext;

    // Input Actions
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* NextPhaseAction;

private:
    void HandleNextPhaseInput();
}; 