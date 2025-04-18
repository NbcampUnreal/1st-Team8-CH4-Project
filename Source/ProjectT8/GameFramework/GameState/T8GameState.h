#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Common/CustomGameState.h"
#include "Net/UnrealNetwork.h"
#include "T8GameState.generated.h"

struct FPlayerStatusUIData;
struct FSlotInfo;

UCLASS()
class PROJECTT8_API AT8GameState : public ACustomGameState
{
	GENERATED_BODY()
	
public:
	void BeginPlay() override;

	UPROPERTY()
	TMap<int32, int32> ActiveTeamCounts;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RemovePlayerFromTeam(int32 TeamID);

	UFUNCTION(BlueprintCallable, Category = "Game")
	bool IsOnlyOneTeamRemaining(int32& OutLastTeamID);

	// Get number of active teams
	UFUNCTION(BlueprintPure, Category = "Game")
	int32 GetActiveTeamCount() const;

	UPROPERTY(Replicated)
	TArray<FPlayerStatusUIData> PlayerUIList;
	

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void InitializePlayerUIList(const TArray<FSlotInfo>& SlotList);
};
