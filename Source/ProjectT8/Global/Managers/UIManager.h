#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Global/Enums/GamePhaseEnum.h"

#include "UIManager.generated.h"

class UUserWidget;

UCLASS()
class PROJECTT8_API UUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void ShowUI(EGamePhase Phase);

private:
    UPROPERTY()
    UUserWidget* CurWidget = nullptr;

    UPROPERTY()
    TMap<EGamePhase, TSubclassOf<UUserWidget>> PhaseWidgetMap;

    void HideCurUI();
};
