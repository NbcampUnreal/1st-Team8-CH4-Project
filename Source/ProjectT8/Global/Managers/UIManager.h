#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Global/Enums/GamePhaseEnum.h"

#include "UIManager.generated.h"

class UUserWidget;
class UPhaseInfoDataAsset;

UCLASS()
class PROJECTT8_API UUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void ShowUI(EGamePhase Phase);

	UFUNCTION(BlueprintCallable)
	void OpenLevelForPhase(EGamePhase Phase);

	void NotifyScreenRemoved(UUserWidget* Widget);

private:
	UPROPERTY()
	UPhaseInfoDataAsset* PhaseInfoDataAsset;

	UPROPERTY()
	TMap<EGamePhase, FName> PhaseTargetNameMap;
	
	UPROPERTY()
    TMap<EGamePhase, TSubclassOf<UUserWidget>> PhaseWidgetMap;

	UPROPERTY()
	UUserWidget* CurWidget = nullptr;

    void HideCurUI();
};
