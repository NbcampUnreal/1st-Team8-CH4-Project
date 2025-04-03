#pragma once
#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Throwable.generated.h"

UCLASS()
class PROJECTT8_API AThrowable : public ABaseItem
{
	GENERATED_BODY()
public:
    AThrowable();

    virtual void Use(class ATestCharacter* Player) override;
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditAnywhere)
    float ThrowSpeed = 1000.0f;

    UPROPERTY(EditAnywhere)
    float ThrowDuration = 2.0f;
};