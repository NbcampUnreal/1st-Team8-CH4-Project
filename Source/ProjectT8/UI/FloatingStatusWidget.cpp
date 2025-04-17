#include "UI/FloatingStatusWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Player/CharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GAS/CharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UFloatingStatusWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UFloatingStatusWidget::NativeDestruct()
{
    Super::NativeDestruct();
}

void UFloatingStatusWidget::SetOwnerCharacter(ACharacter* InCharacter)
{
    if (!InCharacter) return;

    OwnerCharacter = Cast<ACharacterBase>(InCharacter); // Cast 시도

    AbilitySystemComponent = nullptr;
    AttributeSet = nullptr;

    if (IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(InCharacter))
    {
        AbilitySystemComponent = AbilityInterface->GetAbilitySystemComponent();
        if (AbilitySystemComponent)
        {
            const UAttributeSet* BaseAttributeSet = AbilitySystemComponent->GetAttributeSet(UCharacterAttributeSet::StaticClass());
            AttributeSet = const_cast<UCharacterAttributeSet*>(Cast<const UCharacterAttributeSet>(BaseAttributeSet));
            if (AttributeSet)
            {
                UpdateHealthBar(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
                AttributeSet->OnHealthChanged.AddDynamic(this, &UFloatingStatusWidget::UpdateHealthBar);
            }
        }
    }
}


void UFloatingStatusWidget::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
    if (HealthBar)
    {
        float HealthPercent = MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
        HealthBar->SetPercent(HealthPercent);
    }
}

void UFloatingStatusWidget::SetPlayerName(const FString& Name)
{
    if (PlayerNameText)
    {
        PlayerNameText->SetText(FText::FromString(Name));
    }
}

void UFloatingStatusWidget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UFloatingStatusWidget, OwnerCharacter);
}