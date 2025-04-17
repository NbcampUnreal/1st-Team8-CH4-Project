#include "LobbyGameMode.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/PlayerController/LobbyPlayerController.h"
#include "Player/CharacterBase.h"
#include "GameFramework/Common/T8PlayerState.h"

ALobbyGameMode::ALobbyGameMode()
{
    MatchMapName = FName(TEXT("MatchMap"));
    bUseSeamlessTravel = true;
    
    // 기본 클래스 설정
    PlayerControllerClass = ALobbyPlayerController::StaticClass();
}

void ALobbyGameMode::StartGame()
{
    ALobbyGameState* LobbyState = GetGameState<ALobbyGameState>();
    if (LobbyState)
    {
        // 로비의 데이터 처리 (팀 구성, 슬롯 정보 등 필요시 처리)
    }
    UGameplayStatics::OpenLevel(GetWorld(), MatchMapName, true, "listen");
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    UE_LOG(LogTemp, Warning, TEXT("LobbyGameMode::PostLogin called for player: %s"), 
        NewPlayer ? (NewPlayer->PlayerState ? *NewPlayer->PlayerState->GetPlayerName() : TEXT("No PlayerState")) : TEXT("Invalid Player"));
    
    // Call parent implementation first
    Super::PostLogin(NewPlayer);

    if (!NewPlayer || !NewPlayer->PlayerState)
    {
        UE_LOG(LogTemp, Error, TEXT("PostLogin: Invalid player or player state!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("PostLogin: Player %s joined the session"), *NewPlayer->PlayerState->GetPlayerName());

    // Get the LobbyGameState
    ALobbyGameState* LobbyState = GetGameState<ALobbyGameState>();
    if (LobbyState)
    {
        APlayerState* PS = NewPlayer->PlayerState;
        
        // 플레이어가 이미 슬롯에 있는지 확인
        bool bPlayerInSlot = false;
        for (int32 i = 0; i < LobbyState->Slots.Num(); ++i)
        {
            if (LobbyState->Slots[i].PlayerState == PS)
            {
                bPlayerInSlot = true;
                UE_LOG(LogTemp, Log, TEXT("Player %s already in slot %d"), *PS->GetPlayerName(), i);
                break;
            }
        }
        
        // 슬롯에 없으면 할당
        if (!bPlayerInSlot)
        {
            // 첫 번째 빈 슬롯 찾기
            int32 EmptySlot = -1;
            for (int32 i = 0; i < LobbyState->Slots.Num(); ++i)
            {
                if (LobbyState->Slots[i].PlayerState == nullptr && !LobbyState->Slots[i].bIsAI)
                {
                    EmptySlot = i;
                    break;
                }
            }
            
            // 빈 슬롯이 있으면 할당
            if (EmptySlot != -1)
            {
                UE_LOG(LogTemp, Log, TEXT("Assigning player %s to empty slot %d"), *PS->GetPlayerName(), EmptySlot);
                LobbyState->Slots[EmptySlot].PlayerState = PS;
                LobbyState->Slots[EmptySlot].bIsAI = false;
                LobbyState->Slots[EmptySlot].DisplayName = PS->GetPlayerName();
                
                // 변경사항 브로드캐스트
                LobbyState->OnSlotsUpdated.Broadcast();
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("No empty slots available for player %s"), *PS->GetPlayerName());
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PostLogin: LobbyGameState is NULL!"));
    }
}

// Handle logout to remove player from slot when they disconnect
void ALobbyGameMode::Logout(AController* Exiting)
{
    if (APlayerController* PC = Cast<APlayerController>(Exiting))
    {
        if (PC->PlayerState)
        {
            if (ALobbyGameState* LobbyState = GetGameState<ALobbyGameState>())
            {
                LobbyState->RemovePlayerFromSlot(PC->PlayerState);
                UE_LOG(LogTemp, Log, TEXT("RemovePlayerFromSlot called for player: %s"), *PC->PlayerState->GetPlayerName());
            }
        }
    }
    
    Super::Logout(Exiting);
}