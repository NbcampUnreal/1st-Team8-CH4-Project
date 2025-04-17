#include "GameFramework/PlayerController/LoadingPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"


void ALoadingPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetInputMode(FInputModeGameAndUI());
    bShowMouseCursor = true;

    if (IsLocalController())
    {
        if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                Subsystem->AddMappingContext(LoadingMappingContext, 0);
            }
        }
    }
}

void ALoadingPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EInput->BindAction(NextPhaseAction, ETriggerEvent::Triggered, this, &ALoadingPlayerController::HandleNextPhaseInput);
    }
}

void ALoadingPlayerController::HandleNextPhaseInput()
{
    UGameplayStatics::OpenLevel(this, FName("BattleLevel"));
}