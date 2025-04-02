#pragma once
#include "CoreMinimal.h"
#include "Item/Melee.h"
#include "Bat.generated.h"

UCLASS()
class PROJECTT8_API ABat : public AMelee
{
	GENERATED_BODY()
public:
	ABat();
	virtual void Attack() override;
};