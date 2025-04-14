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
	AActor* GetTargetPlayer() const;

	UFUNCTION(BlueprintCallable)
	void RunWeaponSearchQuery();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	

	UFUNCTION(BlueprintCallable)
	void RunTargetSearchQuery();

	UFUNCTION()
	void OnEQSQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnWeaponQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, Category = "EQS")
	UEnvQuery* WeaponQueryTemplate;

	UPROPERTY(EditDefaultsOnly, Category = "EQS")
	UEnvQuery* TargetQueryTemplate;

private:

};
