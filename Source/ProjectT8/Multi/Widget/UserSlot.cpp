#include "Multi/Widget/UserSlot.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Common/T8PlayerState.h"

void UUserSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (CharacterRenderTargetToMaterial)
	{
		SetCharacterImage(CharacterRenderTargetToMaterial);
	}

	SetUserPersonaName();
}

void UUserSlot::SetCharacterName(const FText& NewText)
{
	if (CharacterName)
	{
		CharacterName->SetText(NewText);
	}
}

void UUserSlot::SetCharacterImage(UMaterialInterface* NewMaterial)
{
	if (CharacterImage && NewMaterial)
	{
		CharacterImage->SetBrushFromMaterial(NewMaterial);
	}
}

void UUserSlot::SetUserPersonaName()
{
	FString PersonaName = TEXT("Unknown User");

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (APlayerState* PS = PC->PlayerState)
		{
			if (AT8PlayerState* T8PS = Cast<AT8PlayerState>(PS))
			{
				PersonaName = T8PS->PersonaName;
			}
		}
	}

	if (CharacterName)
	{
		CharacterName->SetText(FText::FromString(PersonaName));
	}
}