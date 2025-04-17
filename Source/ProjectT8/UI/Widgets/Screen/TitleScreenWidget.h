#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/Base/ScreenBaseWidget.h"
#include "TitleScreenWidget.generated.h"


UCLASS()
class PROJECTT8_API UTitleScreenWidget : public UScreenBaseWidget
{
	GENERATED_BODY()
	
public:
    virtual void OnScreenActivated_Implementation() override;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* Anim_TextBlink_Title;

};
