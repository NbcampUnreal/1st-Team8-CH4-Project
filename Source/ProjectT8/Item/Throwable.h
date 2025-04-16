#pragma once
#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Throwable.generated.h"

class USphereComponent;

UCLASS()
class PROJECTT8_API AThrowable : public ABaseItem
{
	GENERATED_BODY()
public:
    AThrowable();

    virtual void Use(class ACharacterBase* Player) override;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UPROPERTY(EditDefaultsOnly, Category = "Throwable")
    float ThrowForce = 2000.0f;

    UPROPERTY(VisibleAnywhere, Category = "Throwable")
    bool bIsThrown = false;

};