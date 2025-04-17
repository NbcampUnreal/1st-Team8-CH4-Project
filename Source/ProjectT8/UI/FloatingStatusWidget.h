#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemInterface.h"
#include "FloatingStatusWidget.generated.h"

class UProgressBar;
class UTextBlock;
class ACharacterBase;
class UAbilitySystemComponent;
class UCharacterAttributeSet;
struct FOnAttributeChangeData;

UCLASS()
class PROJECTT8_API UFloatingStatusWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    void SetOwnerCharacter(ACharacter* InCharacter);

    UFUNCTION()
    void UpdateHealthBar(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintCallable)
    void SetPlayerName(const FString& Name);

    UFUNCTION()
    void SetHealthBarVisibility(bool bIsVisible);

protected:
    // GAS 관련
    void BindAttributeChanges();
    void UnbindAttributeChanges();

    void OnHealthChanged(const FOnAttributeChangeData& Data);
    void OnMaxHealthChanged(const FOnAttributeChangeData& Data);

protected:
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PlayerNameText;

    UPROPERTY()
    ACharacterBase* OwnerCharacter;

    UPROPERTY()
    UAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY()
    UCharacterAttributeSet* AttributeSet;
};