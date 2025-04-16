#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Components/WidgetComponent.h"
#include "CharacterBase.generated.h"

// 캐릭터 죽음 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDeathDelegate, ACharacterBase*, DeadCharacter);

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class ABaseItem;
class UCombatComponent;
class UAbilitySystemComponent;
class UCharacterAttributeSet;
class UGameplayEffect;
class UItemComponent;
struct FInputActionValue;
struct FCharacterAppearanceData;
class UFloatingStatusWidget;

UCLASS()
class PROJECTT8_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();

	// GAS
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Ability System Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", Replicated)
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UCharacterAttributeSet> AttributeSet;

	// Combat
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", Replicated)
	TObjectPtr<class UCombatComponent> CombatComponent;

	UFUNCTION(BlueprintCallable)
	UCombatComponent* GetCombatComponent() const { return CombatComponent; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", Replicated)
	TObjectPtr<class UItemComponent> ItemComponent;

	UFUNCTION(BlueprintCallable)
	UItemComponent* GetItemComponent() const { return ItemComponent; }

	TArray<FGameplayTag> StatusEffectTags;
	void RegisterStatusEffectDelegates();
	UFUNCTION()
	void OnStatusEffectTagChanged(const FGameplayTag Tag, int32 NewCount);

	void ShowStatusWidget(const FGameplayTag& Tag);
	void HideStatusWidget(const FGameplayTag& Tag);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> BurnWidgetClass;

	UPROPERTY()
	UUserWidget* BurnWidgetInstance;

	void InitAbilityActorInfo();

	UFUNCTION(BlueprintCallable)
	void UseItem();

	UFUNCTION(BlueprintCallable)
	void TryInteract();

	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* InteractableActor);

	// Combat
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* AttackMontage;

	UFUNCTION()
	void ApplyApperance(const FCharacterAppearanceData& Data);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth() const;

	// 죽음 관련
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnCharacterDeathDelegate OnCharacterDeath;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
	bool bIsDead = false;

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void Die();

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastDie();

	UFUNCTION(BlueprintPure, Category = "Character")
	virtual bool IsDead() const { return bIsDead; }

	// 팀 정보
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Team")
	int32 TeamNumber;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Team")
	FString PlayerDisplayName;

protected:
	// Input
	virtual void BeginPlay() override;
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

	void Move(const FInputActionValue& Value);
	void SprintStart();
	void SprintEnd();
	void Attack();

	// Input Mapping
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AttackAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* InteractAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* UseItemAction;


	// Camera
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apperance")
	USkeletalMeshComponent* HeadMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apperance")
	USkeletalMeshComponent* AccessoryMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apperance")
	USkeletalMeshComponent* GlovesMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apperance")
	USkeletalMeshComponent* TopMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apperance")
	USkeletalMeshComponent* BottomMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apperance")
	USkeletalMeshComponent* ShoesMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* FloatingStatusWidget;

	UPROPERTY()
	UFloatingStatusWidget* StatusWidget;

	void InitializeFloatingStatusWidget();
	void UpdateHealthUI();

private:
	bool bIsRunning = false;

	const float WalkSpeed = 150.f;
	const float RunSpeed = 600.f;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};