#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/Base/ScreenBaseWidget.h"
#include "PlayingScreenWidget.generated.h"

class UTextBlock;
class UTeamStatusBar;

UCLASS()
class PROJECTT8_API UPlayingScreenWidget : public UScreenBaseWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta=(BindWidget))
	class UTeamStatusBar* TeamStatusBar;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TeamModeText;

	virtual void NativeConstruct() override;
};
