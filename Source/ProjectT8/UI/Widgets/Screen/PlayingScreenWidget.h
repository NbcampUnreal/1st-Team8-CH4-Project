#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/Base/ScreenBaseWidget.h"
#include "PlayingScreenWidget.generated.h"

class UTeamStatusBar;

UCLASS()
class PROJECTT8_API UPlayingScreenWidget : public UScreenBaseWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta=(BindWidget))
	class UTeamStatusBar* TeamStatusBar;

	virtual void NativeConstruct() override;
};
