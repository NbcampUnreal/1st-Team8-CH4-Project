#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FCharacterAppearanceData.h"
#include "CharacterAppearanceSaveGame.generated.h"

UCLASS()
class PROJECTT8_API UCharacterAppearanceSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UCharacterAppearanceSaveGame();

	UPROPERTY(BlueprintReadWrite)
	FCharacterAppearanceData AppearanceData;
};
