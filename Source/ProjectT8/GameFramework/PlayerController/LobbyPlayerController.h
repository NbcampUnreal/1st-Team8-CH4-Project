#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 커스텀 PlayerController 클래스
 * 멀티플레이어 기능 및 RPC를 구현
 */

class UInputMappingContext;
class UInputAction;

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