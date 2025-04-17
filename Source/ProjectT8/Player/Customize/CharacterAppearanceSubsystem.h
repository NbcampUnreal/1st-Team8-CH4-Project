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
	UCharacterAppearanceSubsystem();

	UPROPERTY()
	FCharacterAppearanceData CachedAppearanceData;

	UFUNCTION(BlueprintCallable)
	void SaveAppearance();
	UFUNCTION(BlueprintCallable)
	void LoadAppearance();
	UFUNCTION(BlueprintCallable)
	FString GetDynamicSaveSlot() const;
	UFUNCTION(BlueprintCallable)
	void GenerateRandomAppearance();

	UDataTable* ItemTable;

	UFUNCTION(BlueprintCallable, Category = "Data")
	USkeletalMesh* GetCostumeMeshByID(const FString& ItemID);
};
