#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"

#include "Global/Enums/GamePhaseEnum.h"

#include "PhaseWidgetItem.generated.h"

class UUserWidget;

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class PROJECTT8_API UPhaseWidgetItem : public UObject
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EGamePhase Phase;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UUserWidget> WidgetClass;
};
