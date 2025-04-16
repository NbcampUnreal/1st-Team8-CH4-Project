#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamStatusBar.generated.h"

USTRUCT(BlueprintType)
struct FPlayerStatusUIData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int32 PlayerID;

    UPROPERTY(BlueprintReadWrite)
    FLinearColor TeamColor;

    UPROPERTY(BlueprintReadWrite)
    bool bIsAlive;
};

class UPlayerStatusIcon;

UCLASS()
class PROJECTT8_API UTeamStatusBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void InitPlayerIcons(const TArray<FPlayerStatusUIData>& PlayerList);

protected:
    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* HorizontalBox_Players;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UPlayerStatusIcon> PlayerStatusIconClass;
};
