#include "AI/T8AICharacter.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"

AT8AICharacter::AT8AICharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AT8AICharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(
		DetectionTimer,
		this,
		&AT8AICharacter::DetectNearbyActors,
		1.0f,
		true
	);
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

void AT8AICharacter::ResetCanAttack()
{
	bCanAttack = true;
	bIsAttacking = false;

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		UBlackboardComponent* BB = AICon->GetBlackboardComponent();
		if (BB)
		{
			BB->SetValueAsBool(TEXT("IsAttacking"), false);
		}
	}
}

void AT8AICharacter::DetectNearbyActors()
{
	const float DetectionRadius = 5000.0f;
	const FVector Center = GetActorLocation();

	TArray<FHitResult> HitResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(DetectionRadius);

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Center,
		Center + FVector(0, 0, 1),
		FQuat::Identity,
		ECC_Pawn,
		Sphere
	);

	if (bHit)
	{
		for (auto& Hit : HitResults)
		{
			AActor* Detected = Hit.GetActor();
			if (Detected && Detected != this)
			{
				UE_LOG(LogTemp, Warning, TEXT("감지한 ACtor : %s"), *Detected->GetName());

				if (AAIController* AICon = Cast<AAIController>(GetController()))
				{


					if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
					{
						BB->SetValueAsObject(TEXT("TargetActor"), Detected);
					}
				}
				break;
			}
		}
	}

	DrawDebugSphere(GetWorld(), Center, DetectionRadius, 12, FColor::Blue, false, 1.0f);
}