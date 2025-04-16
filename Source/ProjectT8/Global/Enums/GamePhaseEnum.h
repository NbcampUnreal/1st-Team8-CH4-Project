#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GamePhaseEnum.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	None    UMETA(DisplayName = "None"),	// Initial Phase
	Title	UMETA(DisplayName = "Title"),
	Room	UMETA(DisplayName = "Room"),
	Lobby	UMETA(DisplayName = "Lobby"),
	Loading	UMETA(DisplayName = "Loading"),
	Playing	UMETA(DisplayName = "Playing"),
	Result	UMETA(DisplayName = "Result"),
	Ending	UMETA(DisplayName = "Ending")
};

UCLASS()
class PROJECTT8_API UGamePhaseEnum : public UObject
{
	GENERATED_BODY()
	
};
