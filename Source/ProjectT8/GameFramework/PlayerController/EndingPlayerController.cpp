#include "GameFramework/PlayerController/EndingPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"


void AEndingPlayerController::BeginPlay()
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
                Subsystem->AddMappingContext(EndingMappingContext, 0);
            }
        }
    }
}

void AEndingPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EInput->BindAction(NextPhaseAction, ETriggerEvent::Triggered, this, &AEndingPlayerController::HandleNextPhaseInput);
    }
}

void AEndingPlayerController::HandleNextPhaseInput()
{
    UGameplayStatics::OpenLevel(this, FName("MultiLobbyLevel"));
}