#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatusIcon.generated.h"

UCLASS()
class PROJECTT8_API UPlayerStatusIcon : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void SetStatus(bool bIsAlive, FLinearColor TeamColor);

protected:
    UPROPERTY(meta = (BindWidget))
    class UBorder* Border_TeamOutline;

    UPROPERTY(meta = (BindWidget))
    class UImage* Image_PlayerIcon;
};
