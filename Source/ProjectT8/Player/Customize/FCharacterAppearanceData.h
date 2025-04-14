#pragma once

#include "CoreMinimal.h"
#include "FCharacterAppearanceData.generated.h"

USTRUCT(BlueprintType)
struct FCharacterAppearanceData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName HeadID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName AccessoryID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName GlovesID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName TopID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName BottomID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ShoesID;
};