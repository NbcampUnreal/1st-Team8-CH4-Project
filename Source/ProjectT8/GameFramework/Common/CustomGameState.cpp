


#include "CustomGameState.h"

void ACustomGameState::SetGamePhase(EGamePhase _NewPhase)
{
    if (CurPhase != _NewPhase)
    {
        CurPhase = _NewPhase;
        OnPhaseChanged(_NewPhase);
    }
}

void ACustomGameState::OnPhaseChanged_Implementation(EGamePhase _NewPhase)
{

}