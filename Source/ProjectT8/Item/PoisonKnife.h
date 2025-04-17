#pragma once
#include "CoreMinimal.h"
#include "Item/Throwable.h"
#include "PoisonKnife.generated.h"

UCLASS()
class PROJECTT8_API APoisonKnife : public AThrowable
{
	GENERATED_BODY()

public:
    APoisonKnife();
protected:
    virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    TSubclassOf<UGameplayEffect> PoisonEffect;
};
