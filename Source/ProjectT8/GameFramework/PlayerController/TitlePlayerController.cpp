#include "GameFramework/PlayerController/TitlePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/Common/T8GameInstance.h"
#include "Global/Managers/UIManager.h"


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
    if (UT8GameInstance* GI = Cast<UT8GameInstance>(GetGameInstance()))
    {
        if (UUIManager* UI = GI->GetSubsystem<UUIManager>())
        {
            UI->OpenLevelForPhase(EGamePhase::Room);
        }
    }
}