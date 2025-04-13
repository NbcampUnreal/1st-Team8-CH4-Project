

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UserSlot.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT8_API UUserSlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UBorder* Border;
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HorizontalBox;
	UPROPERTY(meta = (BindWidget))
	UImage* CharacterImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterName;

public:

	UFUNCTION(BlueprintCallable, Category = "MyWidget")
	void SetCharacterName(const FText& NewText);
	UFUNCTION(BlueprintCallable, Category = "MyWidget")
	void SetUserPersonaName();

	UFUNCTION(BlueprintCallable, Category = "MyWidget")
	void SetCharacterImage(UMaterialInterface* NewMaterial);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UMaterialInterface* CharacterRenderTargetToMaterial;
};
