#include "GAS/CharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UCharacterAttributeSet::UCharacterAttributeSet()
{
	Health = 100.f;
	MaxHealth = 100.f;
	Damage = 0.f;
}

void UCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float DamageDone = GetDamage();
		if (DamageDone > 0.0f)
		{
			SetDamage(0.0f);

			const float NewHealth = FMath::Clamp(GetHealth() - DamageDone, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);
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
}

void UCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, MaxHealth, OldValue);
}
