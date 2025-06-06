#include "GameFramework/Common/T8PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "Player/CharacterBase.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Player/Customize/CharacterAppearanceSubsystem.h"

AT8PlayerState::AT8PlayerState()
{
    TeamNumber = 0;
    RetrieveSteamID();
}

void AT8PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AT8PlayerState, PersonaName);
    DOREPLIFETIME(AT8PlayerState, TeamNumber);
    DOREPLIFETIME(AT8PlayerState, ApperanceData);
}

void AT8PlayerState::RetrieveSteamID()
{
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
    if (OnlineSub)
    {
        IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
        if (Identity.IsValid())
        {
            if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
            {
                PersonaName = Identity->GetPlayerNickname(0);
                if (PersonaName.IsEmpty())
                {
                    PersonaName = TEXT("Steam User");
                }
            }
            else
            {
                PersonaName = TEXT("Not Logged In");
            }
        }
        else
        {
            PersonaName = TEXT("Invalid Identity Interface");
        }
    }
    else
    {
        PersonaName = TEXT("Steam Subsystem Unavailable");
    }
}

void AT8PlayerState::OnRep_AppearanceData()
{
    if (ACharacterBase* MyChar = Cast<ACharacterBase>(GetPawn()))
    {
        MyChar->ApplyApperance(ApperanceData);
    }
}