#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EffectComponent.generated.h"

UENUM(BlueprintType)
enum class EEffectType : uint8
{
    None        UMETA(DisplayName = "None"),
    Fire        UMETA(DisplayName = "Fire"),
    Electric    UMETA(DisplayName = "Electric"),
    Ice         UMETA(DisplayName = "Ice"),
    Flash      UMETA(DisplayName = "Flash"),
    Teleport      UMETA(DisplayName = "Teleport"),
    Poison      UMETA(DisplayName = "Poison"),
    SpeedUp     UMETA(DisplayName = "SpeedUp"),
    Invisibility UMETA(DisplayName = "Invisibility"),
    Shield      UMETA(DisplayName = "Shield"),
    Clone       UMETA(DisplayName = "Clone"),
    Heal        UMETA(DisplayName = "Heal"),
};

USTRUCT(BlueprintType)
struct FEffectParams
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Effect")
    float Duration = 0.0f;  // 효과 지속 시간

    UPROPERTY(BlueprintReadWrite, Category = "Effect")
    float Value = 0.0f;     // 데미지, 회복량 등 주요 값

    UPROPERTY(BlueprintReadWrite, Category = "Effect")
    float Interval = 0.0f;  // 지속 데미지 적용 간격

    UPROPERTY(BlueprintReadWrite, Category = "Effect")
    float Multiplier = 1.0f; // 속도 증가 비율 등
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTT8_API UEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEffectComponent();

    UFUNCTION(BlueprintCallable, Category = "Effect")
    void ApplyEffect(EEffectType EffectType, const FEffectParams& Params);

protected:
	virtual void BeginPlay() override;

public:	

    void ApplyFireEffect(float Damage, float Duration, float Interval);
    void ApplyElectricEffect(float Duration);
    void ApplyIceEffect(float Duration);
    void ApplyFlashEffect(float Duration);
	void ApplyTeleportEffect();
    void ApplyPoisonEffect(float Duration);
    void ApplySpeedUpEffect(float Multiplier, float Duration);
    void ApplyInvisibilityEffect(float Duration);
    void ApplyShieldEffect(float Duration);
    void ApplyCloneEffect(float Duration);
    void ApplyHealEffect(float Amount);

protected:
    UPROPERTY(EditAnywhere, Category = "Effect Widgets")
    TSubclassOf<UUserWidget> FlashWidgetClass;

    UPROPERTY(EditAnywhere, Category = "Effect Widgets")
    TSubclassOf<UUserWidget> PoisonWidgetClass;

private:
    void ShowHUDWidget(TSubclassOf<UUserWidget> WidgetClass, float Duration);
};
