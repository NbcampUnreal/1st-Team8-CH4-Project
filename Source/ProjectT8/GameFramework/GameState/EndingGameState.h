#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Common/CustomGameState.h"
#include "EndingGameState.generated.h"


UCLASS()
class PROJECTT8_API AEndingGameState : public ACustomGameState
{
	GENERATED_BODY()

public:
	void BeginPlay() override;
	
};
