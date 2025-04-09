#include "AI/T8AICharacter.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"


AT8AICharacter::AT8AICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	TeamIndicator = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TeamIndicator"));
	TeamIndicator->SetupAttachment(RootComponent);
	TeamIndicator->SetHorizontalAlignment(EHTA_Center);
	TeamIndicator->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	TeamIndicator->SetText(FText::FromString("T0"));
	TeamIndicator->SetTextRenderColor(FColor::White);
	TeamIndicator->SetWorldSize(30.0f);
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

void AT8AICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TeamIndicator)
	{
		APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		if (CameraManager)
		{
			FVector CameraLocation = CameraManager->GetCameraLocation();
			FVector ToCamera = CameraLocation - TeamIndicator->GetComponentLocation();
			FRotator LookRotation = FRotationMatrix::MakeFromX(ToCamera).Rotator();

			LookRotation.Pitch = 0.f;
			LookRotation.Roll = 0.f;

			TeamIndicator->SetWorldRotation(LookRotation);
		}
	}
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
			AActor* HitActor = Hit.GetActor();
			if (APawn* HitPawn = Cast<APawn>(HitActor))
			{
				if (IsEnemy(HitPawn))
				{
					UE_LOG(LogTemp, Warning, TEXT("AI hit Enemy actor : %s"), *HitPawn->GetName());

					if (AT8AICharacter* EnemyAI = Cast<AT8AICharacter>(HitPawn))
					{
						EnemyAI->ApplyDamage(20.0f);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("AI hit friendly actor : %s (무시됨)"), *HitPawn->GetName());
					}
				}
			}
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
	const float DetectionRadius = 1000.0f;
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
			if (Detected && Detected != this && IsEnemy(Detected))
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

				UE_LOG(LogTemp, Warning, TEXT("목표 전환 : %s"), *GetNameSafe(CurrentTarget));
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

bool AT8AICharacter::IsEnemy(AActor* OtherActor) const
{
	if (!OtherActor || OtherActor == this) return false;

	if (!OtherActor->IsA<APawn>()) return false;

	if(const AT8AICharacter* OtherAI = Cast< AT8AICharacter>(OtherActor))
	{
		return OtherAI->GetTeamID() != GetTeamID();
	}

	return true;
}

int32 AT8AICharacter::GetTeamID() const
{
	return TeamID;
}

void AT8AICharacter::SetTeamID(int32 NewID)
{
	TeamID = NewID;

	if (TeamIndicator)
	{
		TeamIndicator->SetText(FText::FromString(FString::Printf(TEXT("T%d"), TeamID)));

		switch (TeamID)
		{
		case 0: TeamIndicator->SetTextRenderColor(FColor::Blue); break;
		case 1: TeamIndicator->SetTextRenderColor(FColor::Red); break;
		case 2: TeamIndicator->SetTextRenderColor(FColor::Green); break;
		default : TeamIndicator->SetTextRenderColor(FColor::White); break;
		}
	}
}

