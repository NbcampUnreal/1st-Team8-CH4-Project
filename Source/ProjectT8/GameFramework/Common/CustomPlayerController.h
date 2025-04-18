#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"

#include "CustomPlayerController.generated.h"

class UUserWidget;

UCLASS()
class PROJECTT8_API ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SetCustomInputMode();

	void SetMouseLocation();

	UUserWidget* CursorWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> CursorWidgetClass;
};
