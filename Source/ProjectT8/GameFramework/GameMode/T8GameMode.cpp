#include "T8GameMode.h"
#include "Player/CharacterBase.h"
#include "GameFramework/Common/T8PlayerState.h"
#include "GameFramework/Character.h"
#include "AI/T8AICharacter.h"
#include "AI/T8AIController.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "GameFramework/Common/T8GameInstance.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Player/T8PlayerController.h"


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

    AT8PlayerController* PC = Cast<AT8PlayerController>(NewPlayer);
    if (PC)
    {
        PC->Client_TriggerSendAppearance();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ERROR AT8GameMode-AT8PlayerController Is nullptr"));
    }
}



void AT8GameMode::NotifyPlayerDeath_Implementation(ACharacter* DeadCharacter)
{
	// 여기에서 게임 로직에 따른 죽음 처리를 구현
	// 예: 점수 계산, 게임 종료 조건 체크 등
	
	// CharacterBase로 캐스팅이 필요한 경우
	if (ACharacterBase* BaseCharacter = Cast<ACharacterBase>(DeadCharacter))
	{
		// CharacterBase 특정 기능 사용
	}
    else if (AT8AICharacter* AICharacter = Cast<AT8AICharacter>(DeadCharacter))
    {
        // AI 전용 로직
    }
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
