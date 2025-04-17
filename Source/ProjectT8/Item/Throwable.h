#pragma once
#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Throwable.generated.h"

class USphereComponent;
class UGameplayEffect;
class UProjectileMovementComponent;

UCLASS()
class PROJECTT8_API AThrowable : public ABaseItem
{
	GENERATED_BODY()
public:
    AThrowable();

    virtual void Use(class ACharacterBase* Player) override;
    USphereComponent* GetEffectCollision() { return EffectCollision; }

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UPROPERTY(VisibleAnywhere)
    UProjectileMovementComponent* ProjectileMovement;

    virtual TSubclassOf<UGameplayEffect> GetEffectToApply() const { return nullptr; }

    UPROPERTY(EditDefaultsOnly, Category = "Throwable")
    float ThrowForce = 1000.0f;

    UPROPERTY(VisibleAnywhere, Category = "Throwable")
    bool bIsThrown = false;

    UPROPERTY(VisibleAnywhere, Category = "Throwable")
    USphereComponent* EffectCollision;

    UPROPERTY(EditDefaultsOnly, Category = "Throwable")
    float EffectRadius;
};