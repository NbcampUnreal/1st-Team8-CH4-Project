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

    virtual void Use(class ACharacterBase* Player) override;
    virtual void ApplyEffect(ACharacterBase* Target);
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
    virtual void Explode();

protected:
    UPROPERTY(EditAnywhere)
    float ThrowForce = 1000.0f;
    UPROPERTY(EditAnywhere)
    float ThrowDuration = 0.0f;
    UPROPERTY(EditAnywhere)
    float EffectRadius = 500.0f; 
    bool bIsThrown = false;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};