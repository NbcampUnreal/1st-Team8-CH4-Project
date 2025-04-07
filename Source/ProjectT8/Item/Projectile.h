#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class PROJECTT8_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void ApplyEffect(class ACharacterBase* Target);

protected:
    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* ProjectileMesh;

    UPROPERTY(VisibleAnywhere)
    class USphereComponent* CollisionSphere;

    UPROPERTY(EditAnywhere)
    float LifeSpan = 2.0f;

    UPROPERTY(EditAnywhere)
    float MoveSpeed = 1000.0f;

    UFUNCTION()
    void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};