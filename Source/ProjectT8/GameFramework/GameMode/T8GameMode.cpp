#include "T8GameMode.h"
#include "Player/CharacterBase.h"
#include "GameFramework/Common/T8PlayerState.h"

AT8GameMode::AT8GameMode()
{	
	DefaultPawnClass = ACharacterBase::StaticClass();
}

void AT8GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ACharacterBase* MyCharacter = Cast<ACharacterBase>(NewPlayer->GetPawn());
	AT8PlayerState* PS = NewPlayer->GetPlayerState<AT8PlayerState>();

	if (MyCharacter && PS)
	{
		UE_LOG(LogTemp, Display, TEXT(""));
		MyCharacter->ApplyApperance(PS->ApperanceData);
	}
}
