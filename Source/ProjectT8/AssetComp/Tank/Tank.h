

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tank.generated.h"

UCLASS()
class PROJECTT8_API ATank : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void TurnTurret(FVector LookAtTartget);

	void Fire();

	void Aiming();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank")
	UStaticMeshComponent* TankMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank")
	UStaticMeshComponent* TurretMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank")
	UStaticMeshComponent* CannonMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank")
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, Category = "Tank")
	USceneComponent* ProjectileSpawnPoint;

	class ACharacterBase* PlayerCharacter;

	FRotator DefalutRot;

	FTimerHandle TurnTimerHandle;
	FTimerHandle FireTimerHandle;
	FTimerDelegate Delegate;

	UPROPERTY(EditAnywhere)
	float TurnRate = 5.f;
	UPROPERTY(EditAnywhere)
	float FireRate = 3.f;
	UPROPERTY(EditAnywhere)
	float Distance;
	UPROPERTY(VisibleAnywhere)
	float RollValue;

	bool bCanAiming = false;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class ATankProjectile> ProjectileClass;
};
