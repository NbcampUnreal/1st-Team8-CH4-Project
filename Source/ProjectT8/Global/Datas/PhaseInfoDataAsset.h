#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "UI/Datas/PhaseInfoItem.h"

#include "PhaseInfoDataAsset.generated.h"


UCLASS(BlueprintType)
class PROJECTT8_API UPhaseInfoDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	TArray<UPhaseInfoItem*> PhaseInfos;
};
