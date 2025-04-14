#pragma once

#include "CoreMinimal.h"
#include "FCharacterAppearanceData.generated.h"

USTRUCT(BlueprintType)
struct FCharacterAppearanceData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString HeadID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString AccessoryID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString GlovesID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString TopID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString BottomID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ShoesID;
};