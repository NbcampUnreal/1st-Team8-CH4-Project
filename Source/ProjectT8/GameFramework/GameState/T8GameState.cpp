#include "GameFramework/GameState/T8GameState.h"


void AT8GameState::BeginPlay()
{
    Super::BeginPlay();

    SetGamePhase(EGamePhase::Playing);
}
