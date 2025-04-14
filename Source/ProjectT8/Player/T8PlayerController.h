#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "T8PlayerController.generated.h"

UCLASS()
class PROJECTT8_API AT8PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	void ApplyAppearanceToCharacter();
};
