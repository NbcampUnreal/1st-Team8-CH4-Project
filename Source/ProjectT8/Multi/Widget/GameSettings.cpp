#include "GameSettings.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "GameFramework/LobbyGameState.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UGameSettings::NativeConstruct()
{
    Super::NativeConstruct();

    // 현재 월드의 SlotStructure(GameState) 가져오기
    LobbyState = GetWorld()->GetGameState<ALobbyGameState>();
    if (LobbyState)
    {
        // 초기 UI 텍스트 설정 (현재 팀 모드, 맵)
        ETeamSetup currentMode = LobbyState->TeamSetup;
        FText ModeText;
        switch (currentMode)
        {
        case ETeamSetup::FreeForAll: ModeText = FText::FromString(TEXT("Free-For-All")); break;
        case ETeamSetup::TwoTeams:   ModeText = FText::FromString(TEXT("Two Teams"));    break;
        case ETeamSetup::FourTeams:  ModeText = FText::FromString(TEXT("Four Teams"));   break;
        }
        if (TeamModeText) TeamModeText->SetText(ModeText);

        if (MapText) MapText->SetText(FText::FromString(LobbyState->SelectedMap));

        LobbyState->OnTeamModeChanged.AddDynamic(this, &UGameSettings::OnTeamModeButtonClicked);
        LobbyState->OnSelectedMapChanged.AddDynamic(this, &UGameSettings::OnMapButtonClicked);

		// 호스트만 버튼 활성화. 어차피 이 위젯 자체가 호스트 전용이므로 없어도 되는 부분
        APlayerController* PC = GetOwningPlayer();
        bool bIsHost = PC && (PC->GetLocalRole() == ROLE_Authority);
        if (TeamModeButton) TeamModeButton->SetIsEnabled(bIsHost);
        if (MapButton) MapButton->SetIsEnabled(bIsHost);
    }

    // 버튼 클릭 이벤트 연결
    if (TeamModeButton)
    {
        TeamModeButton->OnClicked.AddDynamic(this, &UGameSettings::OnTeamModeButtonClicked);
    }
    if (MapButton)
    {
        MapButton->OnClicked.AddDynamic(this, &UGameSettings::OnMapButtonClicked);
    }
}

void UGameSettings::OnTeamModeButtonClicked()
{
    if (!LobbyState) return;

    APlayerController* PC = GetOwningPlayer();
    bool bIsHost = PC && (PC->GetLocalRole() == ROLE_Authority);
    if (!bIsHost)
        return;
    // 팀 모드 버튼 클릭 -> 다음 팀 모드로 순환 // 버튼 방식이 불편한가?
    ETeamSetup NewMode;
    switch (LobbyState->TeamSetup)
    {
    case ETeamSetup::FreeForAll: NewMode = ETeamSetup::TwoTeams; break;
    case ETeamSetup::TwoTeams:   NewMode = ETeamSetup::FourTeams; break;
    case ETeamSetup::FourTeams:  NewMode = ETeamSetup::FreeForAll; break;
    default: NewMode = ETeamSetup::FreeForAll; break;
    }
    LobbyState->SetTeamSetup(NewMode);

    // 로컬 UI 텍스트 업데이트
    FText ModeText;
    switch (NewMode)
    {
    case ETeamSetup::FreeForAll: ModeText = FText::FromString(TEXT("Free-For-All")); break;
    case ETeamSetup::TwoTeams:   ModeText = FText::FromString(TEXT("Two Teams"));    break;
    case ETeamSetup::FourTeams:  ModeText = FText::FromString(TEXT("Four Teams"));   break;
    }
    if (TeamModeText)
        TeamModeText->SetText(ModeText);
}

void UGameSettings::OnMapButtonClicked()
{
    if (!LobbyState) return;
    APlayerController* PC = GetOwningPlayer();
    bool bIsHost = PC && (PC->GetLocalRole() == ROLE_Authority);
    if (!bIsHost) return;

    LobbyState->CycleMapChoice();
    if (MapText) MapText->SetText(FText::FromString(LobbyState->SelectedMap));
}
