#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "T8AIController.generated.h"


UCLASS()
class PROJECTT8_API AT8AIController : public AAIController
{
	GENERATED_BODY()

public:
	AT8AIController();

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTarget() const;

	UFUNCTION(BlueprintCallable)
	void RunWeaponSearchQuery();

	void RunTargetPriorityQuery();

	UFUNCTION()
	void RequestFindNewTarget();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnWeaponQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnTargetPriorityQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UEnvQuery* TargetSearchQuery;

	UPROPERTY(EditDefaultsOnly, Category = "EQS")
	UEnvQuery* WeaponQueryTemplate;

	UPROPERTY(EditDefaultsOnly, Category = "EQS")
	UEnvQuery* PriorityTargetQueryTemplate;

	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	float LastTargetSetTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Targeting")
	float TargetHoldTime = 3.0f;

	

private:

};
