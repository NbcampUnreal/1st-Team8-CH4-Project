#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Common/CustomGameState.h"
#include "T8GameState.generated.h"


UCLASS()
class PROJECTT8_API AT8GameState : public ACustomGameState
{
	GENERATED_BODY()
	
public:
	void BeginPlay() override;

};
