#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "T8AICharacter.generated.h"

UCLASS()
class PROJECTT8_API AT8AICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AT8AICharacter();

	UFUNCTION()
	void PerformAttackHitCheck();

protected:
	virtual void BeginPlay() override;
};
