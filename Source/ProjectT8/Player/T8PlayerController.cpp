#include "Player/T8PlayerController.h"
#include "Player/Customize/CharacterAppearanceSaveGame.h"
#include "Player/Customize/CharacterAppearanceSubsystem.h"
#include "Player/Customize/FCharacterAppearanceData.h"
#include "Player/CharacterBase.h"

void AT8PlayerController::BeginPlay()
{
	Super::BeginPlay();

	UCharacterAppearanceSubsystem* ApperanceSybsystem = GetGameInstance()->GetSubsystem<UCharacterAppearanceSubsystem>();
	if (ApperanceSybsystem)
	{
		ApperanceSybsystem->LoadAppearance();
	}

	if (GetPawn())
	{
		ApplyAppearanceToCharacter();
	}
}

void AT8PlayerController::ApplyAppearanceToCharacter()
{
	UCharacterAppearanceSubsystem* ApperanceSubsystem = GetGameInstance()->GetSubsystem<UCharacterAppearanceSubsystem>();
	if (!ApperanceSubsystem) return;

	ACharacterBase* MyCharacter = Cast<ACharacterBase>(GetPawn());
	if (!MyCharacter) return;

	const FCharacterAppearanceData& Data = ApperanceSubsystem->CachedAppearanceData;

	MyCharacter->ApplyApperance(Data);
}
