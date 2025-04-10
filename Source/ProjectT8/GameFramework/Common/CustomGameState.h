

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Global/Enums/GamePhaseEnum.h"
#include "CustomGameState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT8_API ACustomGameState : public AGameState
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadOnly)
    EGamePhase CurPhase;

    UFUNCTION(BlueprintCallable)
    void SetGamePhase(EGamePhase _NewPhase);

    UFUNCTION(BlueprintNativeEvent)
    void OnPhaseChanged(EGamePhase _NewPhase);
};
