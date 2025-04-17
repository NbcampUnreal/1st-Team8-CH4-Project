#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Global/Enums/GamePhaseEnum.h"

#include "PhaseInfoItem.generated.h"

class UUserWidget;

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class PROJECTT8_API UPhaseInfoItem : public UObject
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EGamePhase Phase;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName MapName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UUserWidget> WidgetClass;
};
