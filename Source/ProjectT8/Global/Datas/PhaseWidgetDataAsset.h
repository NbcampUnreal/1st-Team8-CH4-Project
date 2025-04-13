#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "PhaseWidgetDataAsset.generated.h"

class UPhaseWidgetItem;

UCLASS(BlueprintType)
class PROJECTT8_API UPhaseWidgetDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UPhaseWidgetItem*> PhaseWidgets;
};
