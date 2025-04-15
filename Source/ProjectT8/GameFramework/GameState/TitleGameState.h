#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Common/CustomGameState.h"
#include "TitleGameState.generated.h"


UCLASS()
class PROJECTT8_API ATitleGameState : public ACustomGameState
{
	GENERATED_BODY()
	
public:
	void BeginPlay() override;

	void InitPhaseAfterDelay();
};
