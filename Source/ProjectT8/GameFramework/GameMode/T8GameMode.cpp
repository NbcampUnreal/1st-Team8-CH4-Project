#include "T8GameMode.h"
#include "Player/CharacterBase.h"
#include "GameFramework/Common/T8PlayerState.h"
#include "GameFramework/Character.h"
#include "AI/T8AICharacter.h"
#include "AI/T8AIController.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "GameFramework/Common/T8GameInstance.h"
#include "GameFramework/GameState/T8GameState.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"


AT8GameMode::AT8GameMode()
{	
	DefaultPawnClass = ACharacterBase::StaticClass();
}


void AT8GameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("T8GameMode::BeginPlay() 실행됨"));

    if (!HasAuthority()) return;

    // GameInstance에서 저장된 슬롯 배열 가져오기
    UT8GameInstance* GameInstance = Cast<UT8GameInstance>(GetGameInstance());
    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("T8GameInstance 캐스팅 실패"));
        return;
    }

    const TArray<FSlotInfo>& SavedSlots = GameInstance->GetSavedLobbySlots();
    UE_LOG(LogTemp, Warning, TEXT("저장된 슬롯 수: %d"), SavedSlots.Num());

    // 각 슬롯을 기준으로 AI 스폰
    for (const FSlotInfo& Slot : SavedSlots)
    {
        if (Slot.bIsAI)
        {
            SpawnAIFromSlotData(Slot);
        }
    }
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



void AT8GameMode::NotifyPlayerDeath_Implementation(ACharacter* DeadCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("NotifyPlayerDeath_Implementation called for %s"), *DeadCharacter->GetName());
	int32 TeamID = -1;

	if (ACharacterBase* BaseCharacter = Cast<ACharacterBase>(DeadCharacter))
	{
        TeamID = BaseCharacter->TeamNumber;
        BaseCharacter->bIsDead = true;
	}
    else if (AT8AICharacter* AICharacter = Cast<AT8AICharacter>(DeadCharacter))
    {
        TeamID = AICharacter->GetTeamID();
		AICharacter->bIsDead = true;
    }

    if (TeamID >= 0)
    {
		if (AT8GameState* GameState = GetGameState<AT8GameState>())
		{
			GameState->RemovePlayerFromTeam(TeamID);
            CheckGameEnd();
		}
    }
}

bool AT8GameMode::CheckGameEnd()
{
    if (AT8GameState* GameState = GetGameState<AT8GameState>())
    {
		int32 WinningTeamID = -1;
		if (GameState->IsOnlyOneTeamRemaining(WinningTeamID))
		{
			UE_LOG(LogTemp, Warning, TEXT("Game Over: Team %d Won!"), WinningTeamID);

            FTimerHandle GameEndTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                GameEndTimerHandle,
                [this]()
            {
                // 여기 게임 종료 UI나 뭔가 처리하는 로직 추가
            }, 3.f, false);

			return true;
		}
    }

    return false;
}

void AT8GameMode::SpawnAllAIFromLobbySlots()
{
    ALobbyGameState* LobbyState = GetGameState<ALobbyGameState>();
    if (!LobbyState) return;

    for (int32 i = 0; i < LobbyState->Slots.Num(); ++i)
    {
        if (LobbyState->Slots[i].bIsAI)
        {
            SpawnAIFromSlot(i);
        }
    }
}

void AT8GameMode::SpawnAIFromSlot(int32 SlotIndex)
{
    ALobbyGameState* LobbyState = GetGameState<ALobbyGameState>();
    if (!LobbyState || !LobbyState->Slots.IsValidIndex(SlotIndex))
    {
        return;
    }

    const FSlotInfo& Slot = LobbyState->Slots[SlotIndex];

    if (!Slot.bIsAI || !AIClass || !AIControllerClass)
    {
        return;
    }

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    FNavLocation NavLocation;

    if (!NavSystem || !NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 2000.f, NavLocation))
    {
        return;
    }

    FVector SpawnLocation = NavLocation.Location + FVector(0.f, 0.f, 100.f);
    FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);

    AT8AICharacter* AI = GetWorld()->SpawnActor<AT8AICharacter>(AIClass, SpawnTransform);
    if (!AI)
    {
        return;
    }

    AI->SetDisplayName(Slot.DisplayName);

    AT8AIController* AICon = GetWorld()->SpawnActor<AT8AIController>(AIControllerClass, SpawnTransform);
    if (AICon)
    {
        AICon->Possess(AI);
    }

    UE_LOG(LogTemp, Log, TEXT("AI Spawned: %s at %s"), *Slot.DisplayName, *SpawnLocation.ToString());
}

void AT8GameMode::SpawnAIFromSlotData(const FSlotInfo& Slot)
{
    if (!AIClass || !AIControllerClass)
    {
        return;
    }

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSystem)
    {
        return;
    }

    FNavLocation NavLocation;
    if (!NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 2000.f, NavLocation))
    {
        return;
    }

    FVector SpawnLocation = NavLocation.Location + FVector(0.f, 0.f, 100.f);
    FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);

    AT8AICharacter* AI = GetWorld()->SpawnActor<AT8AICharacter>(AIClass, SpawnTransform);
    if (!AI)
    {
        return;
    }

    AI->SetDisplayName(Slot.DisplayName);
    AI->SetTeamID(Slot.TeamNumber);

    AT8AIController* AICon = GetWorld()->SpawnActor<AT8AIController>(AIControllerClass, SpawnTransform);
    if (AICon)
    {
        AICon->Possess(AI);
    }
}
