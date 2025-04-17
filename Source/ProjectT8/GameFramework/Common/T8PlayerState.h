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
    
    UPROPERTY(Replicated)
    FCharacterAppearanceData ApperanceData;

    UFUNCTION(BlueprintCallable, Category = "Steam")
    void RetrieveSteamID();

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
