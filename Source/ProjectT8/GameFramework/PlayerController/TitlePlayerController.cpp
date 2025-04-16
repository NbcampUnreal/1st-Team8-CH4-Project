#include "GameFramework/PlayerController/TitlePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"


void ATitlePlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetInputMode(FInputModeGameAndUI());
    bShowMouseCursor = false;

    if (IsLocalController())
    {
        if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                Subsystem->AddMappingContext(TitleMappingContext, 0);
            }
        }
    }
}

void ATitlePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    if (UEnhancedInputComponent* EInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EInput->BindAction(StartGameAction, ETriggerEvent::Triggered, this, &ATitlePlayerController::HandleStartInput);
    }
}

void ATitlePlayerController::HandleStartInput()
{
    UGameplayStatics::OpenLevel(this, FName("PrivateLevel"));
}