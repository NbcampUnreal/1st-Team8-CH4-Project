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

void UFloatingStatusWidget::SetOwnerCharacter(ACharacterBase* Character)
{
    OwnerCharacter = Character;
    if (OwnerCharacter)
    {
        AbilitySystemComponent = OwnerCharacter->GetAbilitySystemComponent();
        if (AbilitySystemComponent)
        {
            const UAttributeSet* BaseAttributeSet = AbilitySystemComponent->GetAttributeSet(UCharacterAttributeSet::StaticClass());
            AttributeSet = const_cast<UCharacterAttributeSet*>(Cast<const UCharacterAttributeSet>(BaseAttributeSet));
            if (AttributeSet)
            {
                // 초기 HP 설정
                UpdateHealthBar(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
                
                // Health 변경 델리게이트 바인딩
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