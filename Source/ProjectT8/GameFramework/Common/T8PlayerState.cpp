#include "GameFramework/Common/T8PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

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
}

void AT8PlayerState::RetrieveSteamID()
{
    // 온라인 서브시스템(예: Steam) 가져오기
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
    if (OnlineSub)
    {
        // 온라인 아이덴티티 인터페이스 가져오기
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