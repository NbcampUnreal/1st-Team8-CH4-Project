

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartGameButton.generated.h"


class UButton;
/**
 *
 */
UCLASS()
class PROJECTT8_API UStartGameButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UButton* StartGameButton;

	virtual void NativeConstruct() override;

	void OnStartGameButtonClicked();
};
