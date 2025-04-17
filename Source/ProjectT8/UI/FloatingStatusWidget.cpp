#include "UI/FloatingStatusWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Player/CharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GAS/CharacterAttributeSet.h"

void UFloatingStatusWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UFloatingStatusWidget::NativeDestruct()
{
    Super::NativeDestruct();
    UnbindAttributeChanges();
}

void UFloatingStatusWidget::SetOwnerCharacter(ACharacter* InCharacter)
{
    if (!InCharacter) return;

    // 이전 바인딩 해제
    UnbindAttributeChanges();

    OwnerCharacter = Cast<ACharacterBase>(InCharacter);
    if (!OwnerCharacter) return;

    // GAS 컴포넌트 설정
    if (IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(OwnerCharacter))
    {
        AbilitySystemComponent = AbilityInterface->GetAbilitySystemComponent();
        if (AbilitySystemComponent)
        {
            const UAttributeSet* BaseAttributeSet = AbilitySystemComponent->GetAttributeSet(UCharacterAttributeSet::StaticClass());
            AttributeSet = const_cast<UCharacterAttributeSet*>(Cast<const UCharacterAttributeSet>(BaseAttributeSet));
            if (AttributeSet)
            {
                // 초기 상태 설정
                UpdateHealthBar(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
                
                // 델리게이트 바인딩
                BindAttributeChanges();
            }
        }
    }
}

void UFloatingStatusWidget::BindAttributeChanges()
{
    if (AttributeSet)
    {
        AttributeSet->OnHealthChanged.AddDynamic(this, &UFloatingStatusWidget::UpdateHealthBar);
    }
}

void UFloatingStatusWidget::UnbindAttributeChanges()
{
    if (AttributeSet)
    {
        AttributeSet->OnHealthChanged.RemoveDynamic(this, &UFloatingStatusWidget::UpdateHealthBar);
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

void UFloatingStatusWidget::SetHealthBarVisibility(bool bIsVisible)
{
    if (HealthBar)
    {
        HealthBar->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}