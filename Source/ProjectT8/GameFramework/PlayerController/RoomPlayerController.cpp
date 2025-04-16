#include "GameFramework/PlayerController/RoomPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"


void ARoomPlayerController::BeginPlay()
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
                Subsystem->AddMappingContext(RoomMappingContext, 0);
            }
        }
    }
}

void ARoomPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EInput->BindAction(NextPhaseAction, ETriggerEvent::Triggered, this, &ARoomPlayerController::HandleNextPhaseInput);
    }
}

void ARoomPlayerController::HandleNextPhaseInput()
{
    UGameplayStatics::OpenLevel(this, FName("LobbyMap"));
}