#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "Global/Enums/GamePhaseEnum.h"

#include "CustomGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChanged, EGamePhase, NewPhase);

UCLASS()
class PROJECTT8_API ACustomGameState : public AGameState
{
	GENERATED_BODY()
	
public:
    virtual void BeginPlay() override;

    UPROPERTY(BlueprintReadOnly)
    EGamePhase CurPhase = EGamePhase::None;

    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnGamePhaseChanged OnPhaseChanged;

    UFUNCTION(BlueprintCallable)
    void SetGamePhase(EGamePhase _NewPhase);

protected:
    
    // 페이즈 전환시 이벤트 감지
    UFUNCTION(BlueprintNativeEvent, Category = "Game")
    void HandlePhaseChanged(EGamePhase _NewPhase);
    virtual void HandlePhaseChanged_Implementation(EGamePhase _NewPhase);
};

