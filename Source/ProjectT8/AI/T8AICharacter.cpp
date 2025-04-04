#include "AI/T8AICharacter.h"
#include "DrawDebugHelpers.h"


AT8AICharacter::AT8AICharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AT8AICharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AT8AICharacter::PerformAttackHitCheck()
{
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorLocation() * 100.0f;

	FCollisionShape Shape = FCollisionShape::MakeSphere(50.0f);
	TArray<FHitResult> HitResults;

	if (GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECC_Pawn, Shape))
	{
		for (const FHitResult& Hit : HitResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("AI hit actor : %s"), *Hit.GetActor()->GetName());
		}
	}

	DrawDebugSphere(GetWorld(), End, 50.0f, 12, FColor::Red, false, 1.0f);
}