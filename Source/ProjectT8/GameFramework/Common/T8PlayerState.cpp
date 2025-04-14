#include "GameFramework/Common/T8PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "Player/CharacterBase.h"
#include "Interfaces/OnlineIdentityInterface.h"

// 상수 정의 추가
const float AT8PlayerState::ApplyAppearanceCheckInterval = 0.1f;
const float AT8PlayerState::MaxApplyAppearanceWaitTime = 5.0f;

AT8PlayerState::AT8PlayerState()
{
    TeamNumber = 0;
    RetrieveSteamID();
    ElapsedWaitTime = 0.0f;
}

void AT8PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AT8PlayerState, PersonaName);
    DOREPLIFETIME(AT8PlayerState, TeamNumber);
    DOREPLIFETIME(AT8PlayerState, ApperanceData);
}

void AT8PlayerState::OnRep_AppearanceData()
{
    if (!GetPawn())
    {
        StartApplyAppearanceTimer();
        return;
    }

    ACharacterBase* MyChar = Cast<ACharacterBase>(GetPawn());
    if (!MyChar)
    {
        return;
    }

    MyChar->ApplyApperance(ApperanceData);
}

void AT8PlayerState::TryApplyAppearanceWithDelay()
{
    ElapsedWaitTime += ApplyAppearanceCheckInterval;
    
    if (ElapsedWaitTime >= MaxApplyAppearanceWaitTime)
    {
        StopApplyAppearanceTimer();
        return;
    }

    if (!GetPawn() || !Cast<ACharacterBase>(GetPawn()))
    {
        return; // 계속 대기
    }

    Cast<ACharacterBase>(GetPawn())->ApplyApperance(ApperanceData);
    StopApplyAppearanceTimer();
}

void AT8PlayerState::StartApplyAppearanceTimer()
{
    if (!GetWorld()) return;
    
    ElapsedWaitTime = 0.0f;
    GetWorld()->GetTimerManager().SetTimer(
        ApplyAppearanceTimerHandle,
        this,
        &AT8PlayerState::TryApplyAppearanceWithDelay,
        ApplyAppearanceCheckInterval,
        true);
}

void AT8PlayerState::StopApplyAppearanceTimer()
{
    if (!GetWorld()) return;
    
    GetWorld()->GetTimerManager().ClearTimer(ApplyAppearanceTimerHandle);
}

void AT8PlayerState::SetAppearanceData(const FCharacterAppearanceData& NewData)
{
    if (HasAuthority())
    {
        ApperanceData = NewData;
        OnRep_AppearanceData();
    }
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