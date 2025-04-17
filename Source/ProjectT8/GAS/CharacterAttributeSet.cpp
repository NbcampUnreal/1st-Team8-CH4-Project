#include "GAS/CharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "Player/CharacterBase.h"

UCharacterAttributeSet::UCharacterAttributeSet()
{
	Health = 100.f;
	MaxHealth = 100.f;
	Damage = 0.f;
}

void UCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
		UE_LOG(LogTemp, Warning, TEXT("Health PreAttributeChange: %f"), NewValue);
	}
}

void UCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		float NewHealth = FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth());
		SetHealth(NewHealth);
		OnHealthChanged.Broadcast(NewHealth, GetMaxHealth());
		UE_LOG(LogTemp, Warning, TEXT("Health Changed: %f"), NewHealth);

		if (NewHealth <= 0.0f)
		{
			if (ACharacterBase* Character = Cast<ACharacterBase>(GetOwningActor()))
			{
				Character->Die();
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float DamageDone = GetDamage();
		if (DamageDone > 0.0f)
		{
			SetDamage(0.0f);
			const float NewHealth = FMath::Clamp(GetHealth() - DamageDone, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);
			OnHealthChanged.Broadcast(NewHealth, GetMaxHealth());
			UE_LOG(LogTemp, Warning, TEXT("Damage %f !!! Current Health: %f"), DamageDone, NewHealth);

			if (NewHealth <= 0.0f)
			{
				if (ACharacterBase* Character = Cast<ACharacterBase>(GetOwningActor()))
				{
					Character->Die();
				}
			}
		}
	}
}

void UCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCharacterAttributeSet, Health);
	DOREPLIFETIME(UCharacterAttributeSet, MaxHealth);
}

void UCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, Health, OldValue);
	OnHealthChanged.Broadcast(GetHealth(), GetMaxHealth());
	UE_LOG(LogTemp, Warning, TEXT("Health 동기화됨: %f -> %f"), OldValue.GetCurrentValue(), GetHealth());
}

void UCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, MaxHealth, OldValue);
	OnHealthChanged.Broadcast(GetHealth(), GetMaxHealth());
}
