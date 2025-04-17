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

void AT8PlayerController::Client_TriggerSendAppearance_Implementation()
{
	UCharacterAppearanceSubsystem* Sub = GetGameInstance()->GetSubsystem<UCharacterAppearanceSubsystem>();
	if (Sub)
	{
		Server_SendMyAppearance(Sub->CachedAppearanceData);
	}
}

void AT8PlayerController::Server_SendMyAppearance_Implementation(const FCharacterAppearanceData& Data)
{
	AT8PlayerState* PS = GetPlayerState<AT8PlayerState>();
	if (PS)
	{
		PS->ApperanceData = Data;

		if (ACharacterBase* MyChar = Cast<ACharacterBase>(PS->GetPawn()))
		{
			MyChar->ApplyApperance(Data);
		}
	}
}