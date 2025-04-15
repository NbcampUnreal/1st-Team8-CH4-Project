#pragma once

#include "CoreMinimal.h"
#include "Data/DataRow.h"
#include "CostumeItemDataRow.generated.h"

USTRUCT(BlueprintType)
struct PROJECTT8_API FCostumeItemDataRow : public FDataRow
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* SkeletalMesh;

	// 한벌 옷 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsOnePiece = false;
};
