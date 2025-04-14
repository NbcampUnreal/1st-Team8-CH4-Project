#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FCharacterAppearanceData.h"
#include "CharacterAppearanceSubsystem.generated.h"

UCLASS()
class PROJECTT8_API UCharacterAppearanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FCharacterAppearanceData CachedAppearanceData;

	UFUNCTION(BlueprintCallable)
	void SaveAppearance();
	UFUNCTION(BlueprintCallable)
	void LoadAppearance();
};
