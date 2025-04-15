#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UserSlot.generated.h"

class APlayerState;

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
    
    UPROPERTY(meta = (BindWidget))
    UButton* KickButton;

    // Called when the kick button is clicked
    UFUNCTION()
    void OnKickButtonClicked();

public:
    // Set up the slot with all needed information
    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void SetupSlot(const FString& DisplayName, APlayerState* InPlayerState, bool bIsAI, int32 SlotIndex, bool bIsHost);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetCharacterName(const FText& NewText);
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetUserPersonaName();
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetCharacterImage(UMaterialInterface* NewMaterial);
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowKickButton(bool bShow);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UMaterialInterface* CharacterRenderTargetToMaterial;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UMaterialInterface* AICharacterMaterial;
    
private:
    // Reference to player state for this slot
    UPROPERTY()
    APlayerState* PlayerStateRef;
    
    // Slot index in the lobby grid
    int32 SlotIndex;
    
    // Whether this slot contains an AI
    bool bIsAI;
    
    // Whether the local player is the host
    bool bIsHost;
};