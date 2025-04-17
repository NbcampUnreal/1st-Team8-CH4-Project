#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/Base/ScreenBaseWidget.h"
#include "LoadingScreenWidget.generated.h"


UCLASS()
class PROJECTT8_API ULoadingScreenWidget : public UScreenBaseWidget
{
	GENERATED_BODY()
	
public:
    void BindProgressBarFinished();

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* Anim_ProgressBar_Loading;

    UPROPERTY()
    FWidgetAnimationDynamicEvent ProgressBarFinishedDelegate;

    UFUNCTION()
    void HandleProgressBarFinished();

protected:

};
