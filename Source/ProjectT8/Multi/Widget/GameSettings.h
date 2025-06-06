#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "GameSettings.generated.h"

class UTextBlock;
class UButton;
class ALobbyGameState;
class USlotStructure;

UCLASS()
class PROJECTT8_API UGameSettings : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;

protected:
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TeamModeText;
    UPROPERTY(meta = (BindWidget))
    UButton* TeamModeButton;

    UPROPERTY(meta = (BindWidget))
    USlotStructure* SlotStructure;

    UFUNCTION()
    void OnTeamModeButtonClicked();
    
    UFUNCTION()
    void OnTeamModeChanged();
    
    void UpdateTeamModeUI(ETeamSetup TeamMode);

    ALobbyGameState* LobbyState;
};