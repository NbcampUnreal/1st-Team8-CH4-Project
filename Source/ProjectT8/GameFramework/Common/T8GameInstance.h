#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "T8GameInstance.generated.h"

class UPhaseWidgetDataAsset;

UCLASS()
class PROJECTT8_API UT8GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UPhaseWidgetDataAsset* WidgetDataAsset;
};
