#include "Player/EffectComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/CharacterBase.h"
#include "Player/ItemTestCharacter.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

UEffectComponent::UEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEffectComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEffectComponent::ApplyEffect(EEffectType EffectType, const FEffectParams& Params)
{
    switch (EffectType)
    {
    case EEffectType::Fire:
        ApplyFireEffect(Params.Value, Params.Duration, Params.Interval);
        break;
    case EEffectType::Electric:
        ApplyElectricEffect(Params.Duration);
        break;
    case EEffectType::Ice:
        ApplyIceEffect(Params.Duration);
        break;
    case EEffectType::Flash:
        ApplyFlashEffect(Params.Duration);
        break;
    case EEffectType::Teleport:
        ApplyTeleportEffect();
        break;
    case EEffectType::Poison:
        ApplyPoisonEffect(Params.Duration);
        break;
    case EEffectType::SpeedUp:
        ApplySpeedUpEffect(Params.Multiplier, Params.Duration);
        break;
    case EEffectType::Invisibility:
        ApplyInvisibilityEffect(Params.Duration);
        break;
    case EEffectType::Shield:
        ApplyShieldEffect(Params.Duration);
        break;
    case EEffectType::Clone:
        ApplyCloneEffect(Params.Duration);
        break;
    case EEffectType::Heal:
        ApplyHealEffect(Params.Value);
        break;
    default:
        break;
    }
}

void UEffectComponent::ApplyFireEffect(float Damage, float Duration, float Interval)
{
    //틱뎀로직(Damage를 Interval마다 적용)
}

void UEffectComponent::ApplyElectricEffect(float Duration)
{
    if (ACharacterBase* Char = Cast<ACharacterBase>(GetOwner()))
    {
        Char->GetCharacterMovement()->DisableMovement();
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [Char]() {
            Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
            }, Duration, false);
    }
}

void UEffectComponent::ApplyIceEffect(float Duration)
{
    ApplyElectricEffect(Duration);
}

void UEffectComponent::ApplyFlashEffect(float Duration)
{
    if (ACharacterBase* Char = Cast<ACharacterBase>(GetOwner()))
    {
        ShowHUDWidget(FlashWidgetClass, Duration); // HUD
    }
}

void UEffectComponent::ApplyTeleportEffect()
{
    if (ACharacterBase* Char = Cast<ACharacterBase>(GetOwner()))
    {
        FVector RandomLocation = FVector(FMath::RandRange(-1000.0f, 1000.0f), FMath::RandRange(-1000.0f, 1000.0f), Char->GetActorLocation().Z);
        Char->SetActorLocation(RandomLocation); // 랜덤 텔레포트
    }
}

void UEffectComponent::ApplyPoisonEffect(float Duration)
{
    if (ACharacterBase* Char = Cast<ACharacterBase>(GetOwner()))
    {
		ShowHUDWidget(PoisonWidgetClass, Duration); // HUD
    }
}

void UEffectComponent::ApplySpeedUpEffect(float Multiplier, float Duration)
{
    if (ACharacterBase* Char = Cast<ACharacterBase>(GetOwner()))
    {
        float OriginalSpeed = Char->GetCharacterMovement()->MaxWalkSpeed;
        Char->GetCharacterMovement()->MaxWalkSpeed *= Multiplier;
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [Char, OriginalSpeed]() {
            Char->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
            }, Duration, false);
    }
}

void UEffectComponent::ApplyInvisibilityEffect(float Duration)
{

}

void UEffectComponent::ApplyShieldEffect(float Duration)
{

}

void UEffectComponent::ApplyCloneEffect(float Duration)
{

}

void UEffectComponent::ApplyHealEffect(float Amount)
{
    if (AItemTestCharacter* Char = Cast<AItemTestCharacter>(GetOwner()))//test MainCharacter로 변경 必
    {
        Char->ModifyHealth(Amount);
        UE_LOG(LogTemp, Warning, TEXT("Heal effect applied: +%f HP"), Amount);
    }
}

void UEffectComponent::ShowHUDWidget(TSubclassOf<UUserWidget> WidgetClass, float Duration)
{
    if (!WidgetClass) return;

    if (APlayerController* PC = Cast<APlayerController>(Cast<ACharacterBase>(GetOwner())->GetController()))
    {
        if (UUserWidget* Widget = CreateWidget<UUserWidget>(PC, WidgetClass))
        {
            Widget->AddToViewport();
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, [Widget]() {
                Widget->RemoveFromViewport();
                }, Duration, false);
        }
    }
}
