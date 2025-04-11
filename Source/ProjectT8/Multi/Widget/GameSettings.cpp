#include "GameSettings.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "GameFramework/LobbyGameState.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UGameSettings::NativeConstruct()
{
    Super::NativeConstruct();

    // ���� ������ SlotStructure(GameState) ��������
    LobbyState = GetWorld()->GetGameState<ALobbyGameState>();
    if (LobbyState)
    {
        // �ʱ� UI �ؽ�Ʈ ���� (���� �� ���, ��)
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

		// ȣ��Ʈ�� ��ư Ȱ��ȭ. ������ �� ���� ��ü�� ȣ��Ʈ �����̹Ƿ� ��� �Ǵ� �κ�
        APlayerController* PC = GetOwningPlayer();
        bool bIsHost = PC && (PC->GetLocalRole() == ROLE_Authority);
        if (TeamModeButton) TeamModeButton->SetIsEnabled(bIsHost);
        if (MapButton) MapButton->SetIsEnabled(bIsHost);
    }

    // ��ư Ŭ�� �̺�Ʈ ����
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
    // �� ��� ��ư Ŭ�� -> ���� �� ���� ��ȯ // ��ư ����� �����Ѱ�?
    ETeamSetup NewMode;
    switch (LobbyState->TeamSetup)
    {
    case ETeamSetup::FreeForAll: NewMode = ETeamSetup::TwoTeams; break;
    case ETeamSetup::TwoTeams:   NewMode = ETeamSetup::FourTeams; break;
    case ETeamSetup::FourTeams:  NewMode = ETeamSetup::FreeForAll; break;
    default: NewMode = ETeamSetup::FreeForAll; break;
    }
    LobbyState->SetTeamSetup(NewMode);

    // ���� UI �ؽ�Ʈ ������Ʈ
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
