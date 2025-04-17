#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"

#include "T8GameInstance.generated.h"

class UPhaseWidgetDataAsset;

UCLASS()
class PROJECTT8_API UT8GameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UPhaseWidgetDataAsset* WidgetDataAsset;
};
