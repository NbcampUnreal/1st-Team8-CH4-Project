#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScreenBaseWidget.generated.h"


UCLASS()
class PROJECTT8_API UScreenBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintNativeEvent)
    void OnScreenActivated();
    virtual void OnScreenActivated_Implementation();

    UFUNCTION(BlueprintNativeEvent)
    void OnScreenDeactivated();
    virtual void OnScreenDeactivated_Implementation();

    void BindFadeOutFinished();

    UPROPERTY()
    FWidgetAnimationDynamicEvent FadeOutFinishedDelegate;

    UFUNCTION()
    void HandleFadeOutFinished();

protected:
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* Anim_FadeIn_Screen;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* Anim_FadeOut_Screen;
};
