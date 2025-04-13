#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Global/Enums/GamePhaseEnum.h"

#include "PhaseWidgetItem.generated.h"

class UUserWidget;

UCLASS()
class PROJECTT8_API UPhaseWidgetItem : public UObject
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EGamePhase Phase;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UUserWidget> WidgetClass;
};
