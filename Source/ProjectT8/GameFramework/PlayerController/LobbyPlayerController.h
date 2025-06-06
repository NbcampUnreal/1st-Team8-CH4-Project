#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Common/CustomPlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * Lobby에서 플레이어 상호작용을 처리하는 PlayerController
 */

class UInputMappingContext;
class UInputAction;
struct FCharacterAppearanceData;

UCLASS()
class PROJECTT8_API ALobbyPlayerController : public ACustomPlayerController
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
    bool ServerRequestMoveToSlot_Validate(int32 SlotIndex);
    void ServerRequestMoveToSlot_Implementation(int32 SlotIndex);

    // 호스트가 AI를 슬롯에 추가 (UI 등에서 호출)
    UFUNCTION(BlueprintCallable, Category = "Lobby|Admin")
    void AddAIToSlot(int32 SlotIndex);

    // 호스트가 AI를 슬롯에서 제거 (UI 등에서 호출)
    UFUNCTION(BlueprintCallable, Category = "Lobby|Admin")
    void RemoveAIFromSlot(int32 SlotIndex);

    UFUNCTION(Client, Reliable)
    void ClientTravelToPrivateLevel();

    UFUNCTION(Server, Reliable)
    void Server_SendMyAppearance(const FCharacterAppearanceData& MyData);

    UFUNCTION(Client, Reliable)
    void Client_TriggerSendAppearance();

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