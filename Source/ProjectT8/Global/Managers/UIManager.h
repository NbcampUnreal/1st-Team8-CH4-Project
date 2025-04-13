#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Global/Enums/GamePhaseEnum.h"

#include "UIManager.generated.h"

class UUserWidget;
class UPhaseWidgetDataAsset;

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
	UPhaseWidgetDataAsset* WidgetDataAsset;

    UPROPERTY()
    TMap<EGamePhase, TSubclassOf<UUserWidget>> PhaseWidgetMap;

	UPROPERTY()
	UUserWidget* CurWidget = nullptr;

    void HideCurUI();
};
