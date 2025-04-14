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

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnEQSQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, Category = "EQS")
	UEnvQuery* TargetQueryTemplate;

	void RunTargetSearchQuery();
};
