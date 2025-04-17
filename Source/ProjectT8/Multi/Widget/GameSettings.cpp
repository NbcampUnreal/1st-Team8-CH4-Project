#include "GameSettings.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SlotStructure.h"

void UGameSettings::NativeConstruct()
{
    Super::NativeConstruct();

    LobbyState = GetWorld()->GetGameState<ALobbyGameState>();
    if (LobbyState)
    {
        // UI 초기화
        UpdateTeamModeUI(LobbyState->TeamSetup);

        // 델리게이트 바인딩
        LobbyState->OnTeamModeChanged.RemoveAll(this);
        LobbyState->OnTeamModeChanged.AddDynamic(this, &UGameSettings::OnTeamModeChanged);

        // 호스트만 설정 변경 가능
        APlayerController* PC = GetOwningPlayer();
        bool bIsHost = PC && PC->HasAuthority();
        if (TeamModeButton) TeamModeButton->SetIsEnabled(bIsHost);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameSettings: LobbyState is null!"));
    }

    // 버튼 이벤트 바인딩
    if (TeamModeButton)
    {
        TeamModeButton->OnClicked.AddDynamic(this, &UGameSettings::OnTeamModeButtonClicked);
    }
}

void UGameSettings::UpdateTeamModeUI(ETeamSetup TeamMode)
{
    if (!TeamModeText) return;
    
    FText ModeText;
    switch (TeamMode)
    {
    case ETeamSetup::FreeForAll: ModeText = FText::FromString(TEXT("Free-For-All")); break;
    case ETeamSetup::TwoTeams:  ModeText = FText::FromString(TEXT("Two Teams"));    break;
    case ETeamSetup::FourTeams: ModeText = FText::FromString(TEXT("Four Teams"));   break;
    }
    
    TeamModeText->SetText(ModeText);
}

void UGameSettings::OnTeamModeChanged()
{
    if (!LobbyState) return;
    
    // 현재 설정된 팀 모드로 UI 업데이트
    UpdateTeamModeUI(LobbyState->TeamSetup);
    
    UE_LOG(LogTemp, Log, TEXT("Team mode changed to: %d"), (int32)LobbyState->TeamSetup);
}

void UGameSettings::OnTeamModeButtonClicked()
{
    if (!LobbyState) return;
    
    // 호스트 체크
    APlayerController* PC = GetOwningPlayer();
    bool bIsHost = PC && PC->HasAuthority();
    if (!bIsHost) 
    {
        UE_LOG(LogTemp, Warning, TEXT("Only host can change team mode!"));
        return;
    }

    // 다음 팀 모드로 순환
    ETeamSetup NewMode;
    switch (LobbyState->TeamSetup)
    {
    case ETeamSetup::FreeForAll: NewMode = ETeamSetup::TwoTeams; break;
    case ETeamSetup::TwoTeams:  NewMode = ETeamSetup::FourTeams; break;
    case ETeamSetup::FourTeams: NewMode = ETeamSetup::FreeForAll; break;
    default:                     NewMode = ETeamSetup::FreeForAll; break;
    }
    
    // 서버에서만 팀 모드 변경 가능
    LobbyState->SetTeamSetup(NewMode);
    UE_LOG(LogTemp, Log, TEXT("Team mode set to: %d"), (int32)NewMode);
    
    // 주의: SlotStructure.UpdateTeamMode는 직접 호출할 필요 없음
    // LobbyGameState에서 OnTeamModeChanged 델리게이트를 브로드캐스트하여 
    // SlotStructure가 자동으로 업데이트되도록 함
}
