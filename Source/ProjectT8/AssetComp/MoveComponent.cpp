


#include "AssetComp/MoveComponent.h"
// Sets default values for this component's properties
UMoveComponent::UMoveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	StartLot = GetOwner()->GetActorLocation();
}


// Called every frame
void UMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	CurrentLot = GetOwner()->GetActorLocation();

	CurrentLot = CurrentLot + (MoveVelocity * DeltaTime);

	GetOwner()->SetActorLocation(CurrentLot);

	float DistMove = FVector::Dist(StartLot, CurrentLot);
	if (DistMove > MoveDist)
	{
		FVector MoveDirection = MoveVelocity.GetSafeNormal();
		StartLot = StartLot + MoveDirection * MoveDist;
		GetOwner()->SetActorLocation(StartLot);
		MoveVelocity = -MoveVelocity;
	}
	// ...
}

