#include "GameFramework/Common/CustomPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"


void ACustomPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetCustomInputMode();

    if (CursorWidgetClass)
    {
        CursorWidget = CreateWidget<UUserWidget>(this, CursorWidgetClass);
        CursorWidget->SetIsFocusable(false);
        CursorWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

        if (CursorWidget)
        {
            CursorWidget->AddToViewport();
        }
    }
}

void ACustomPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    SetShowMouseCursor(false);
    SetMouseLocation();
}

void ACustomPlayerController::SetCustomInputMode()
{
    /*bShowMouseCursor = false;
    SetInputMode(FInputModeGameAndUI());*/

    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(true);
    InputMode.SetWidgetToFocus(nullptr);
    SetInputMode(InputMode);

    SetShowMouseCursor(false);
}

void ACustomPlayerController::SetMouseLocation()
{
    /*if (CursorWidget)
    {
        float LocationX, LocationY;
        GetMousePosition(LocationX, LocationY);

        CursorWidget->SetPositionInViewport(FVector2D(LocationX, LocationY), false);
    }*/

    FVector2D MousePosition;
    UWidgetLayoutLibrary::GetMousePositionScaledByDPI(this, MousePosition.X, MousePosition.Y);

    if (CursorWidget)
    {
        CursorWidget->SetPositionInViewport(MousePosition, false);
    }
}
