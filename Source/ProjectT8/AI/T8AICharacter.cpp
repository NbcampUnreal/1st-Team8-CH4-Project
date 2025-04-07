#include "AI/T8AICharacter.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"

AT8AICharacter::AT8AICharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AT8AICharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;

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

	AActor* Closest = nullptr;
	float ClosestDist = TNumericLimits<float>::Max();

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* Detected = Hit.GetActor();
			if (Detected && Detected != this)
			{
				float Dist = FVector::Dist(Detected->GetActorLocation(), GetActorLocation());
				if (Dist < ClosestDist)
				{
					Closest = Detected;
					ClosestDist = Dist;
				}
			}
		}
	}

	if (CurrentTarget == nullptr)
	{
		CurrentTarget = Closest;
		TargetTrackTime = 0.0f;
	}
	else if (Closest == CurrentTarget)
	{
		TargetTrackTime += 1.0f;
		PotentialTarget = nullptr;
		PotentialTargetTime = 0.0f;
	}
	else
	{
		if (PotentialTarget == Closest)
		{
			PotentialTargetTime += 1.0f;
			if (PotentialTargetTime >= TargetStickTime)
			{
				UE_LOG(LogTemp, Warning, TEXT("타겟 전환 조건 충족! 기존: %s -> 새로운: %s"),
					*GetNameSafe(CurrentTarget), *GetNameSafe(PotentialTarget));

				CurrentTarget = PotentialTarget;
				TargetTrackTime = 0.0f;
				PotentialTarget = nullptr;
				PotentialTargetTime = 0.0f;

				if (AAIController* AICon = Cast<AAIController>(GetController()))
				{
					if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
					{
						BB->SetValueAsObject(TEXT("TargetActor"), CurrentTarget);
					}

					AICon->StopMovement();
					if (AICon->BrainComponent)
					{
						AICon->BrainComponent->RestartLogic();
					}
				}

				UE_LOG(LogTemp, Warning, TEXT("목표 전환 : %s"), *CurrentTarget->GetName());
			}
		}
		else
		{
			PotentialTarget = Closest;
			PotentialTargetTime = 0.0f;
		}
	}
	if (CurrentTarget)
	{
		if (AAIController* AICon = Cast<AAIController>(GetController()))
		{
			if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
			{
				BB->SetValueAsObject(TEXT("TargetActor"), CurrentTarget);
			}
		}
	}

	DrawDebugSphere(GetWorld(), Center, DetectionRadius, 12, FColor::Blue, false, 1.0f);
}

void AT8AICharacter::ApplyDamage(float DamageAmount)
{
	CurrentHP -= DamageAmount;
	UE_LOG(LogTemp, Warning, TEXT("AI 체력 : %.1f"), CurrentHP);

	if (CurrentHP <= 0.0f)
	{
		Die();
	}
}

void AT8AICharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("AI 사망 처리 시작"));

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	DetachFromControllerPendingDestroy();

	SetLifeSpan(5.0f);
}