#include "Player/T8PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Common/T8PlayerState.h"
#include "Player/Customize/CharacterAppearanceSaveGame.h"
#include "Player/Customize/CharacterAppearanceSubsystem.h"
#include "Player/Customize/FCharacterAppearanceData.h"
#include "Player/CharacterBase.h"

void AT8PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())  // 로컬 컨트롤러일 때만 실행
	{
		UCharacterAppearanceSubsystem* ApperanceSybsystem = GetGameInstance()->GetSubsystem<UCharacterAppearanceSubsystem>();
		if (ApperanceSybsystem)
		{
			ApperanceSybsystem->LoadAppearance();
			UE_LOG(LogTemp, Warning, TEXT("Calling ServerSetAppearanceData from client"));
			ServerSetAppearanceData(ApperanceSybsystem->CachedAppearanceData);
		}
	}

	// Input
	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;

	if (IsLocalController())
	{
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(PlayingMappingContext, 0);
			}
		}
	}
}

void AT8PlayerController::ServerSetAppearanceData_Implementation(const FCharacterAppearanceData& InData)
{
	UE_LOG(LogTemp, Warning, TEXT("[Server] ServerSetAppearanceData_Implementation"));
	
	if (AT8PlayerState* PS = GetPlayerState<AT8PlayerState>())
	{
		PS->SetAppearanceData(InData);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Server] PlayerState is null in ServerSetAppearanceData_Implementation"));
	}
}

void AT8PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EInput->BindAction(NextPhaseAction, ETriggerEvent::Triggered, this, &AT8PlayerController::HandleNextPhaseInput);
	}
}

void AT8PlayerController::HandleNextPhaseInput()
{
	UGameplayStatics::OpenLevel(this, FName("ResultMap"));
}