#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Player/Customize/FCharacterAppearanceData.h"
#include "T8PlayerState.generated.h"

UCLASS()
class PROJECTT8_API AT8PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
    AT8PlayerState();

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
    FString PersonaName;
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    int32 TeamNumber;
    
    UPROPERTY(ReplicatedUsing = OnRep_AppearanceData)
    FCharacterAppearanceData ApperanceData;

    UFUNCTION()
    void OnRep_AppearanceData();

    void SetAppearanceData(const FCharacterAppearanceData& NewData);

    UFUNCTION(BlueprintCallable, Category = "Steam")
    void RetrieveSteamID();

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    // 지연된 외형 적용을 위한 함수들
    void TryApplyAppearanceWithDelay();
    void StartApplyAppearanceTimer();
    void StopApplyAppearanceTimer();
    
    FTimerHandle ApplyAppearanceTimerHandle;
    static const float ApplyAppearanceCheckInterval;
    static const float MaxApplyAppearanceWaitTime;
    float ElapsedWaitTime;
};
