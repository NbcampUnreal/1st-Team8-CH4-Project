#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "UI/Datas/PhaseWidgetItem.h"

#include "PhaseWidgetDataAsset.generated.h"


UCLASS(BlueprintType)
class PROJECTT8_API UPhaseWidgetDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	TArray<UPhaseWidgetItem*> PhaseWidgets;
};
