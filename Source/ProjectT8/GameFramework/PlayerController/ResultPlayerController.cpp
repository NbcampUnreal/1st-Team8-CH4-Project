#include "GameFramework/PlayerController/ResultPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"


void AResultPlayerController::BeginPlay()
{
    Super::BeginPlay();

    //SetInputMode(FInputModeGameAndUI());
    //bShowMouseCursor = true;

    if (IsLocalController())
    {
        if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                Subsystem->AddMappingContext(ResultMappingContext, 0);
            }
        }
    }
}

void AResultPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EInput->BindAction(NextPhaseAction, ETriggerEvent::Triggered, this, &AResultPlayerController::HandleNextPhaseInput);
    }
}

void AResultPlayerController::HandleNextPhaseInput()
{
    UGameplayStatics::OpenLevel(this, FName("EndingMap"));
}