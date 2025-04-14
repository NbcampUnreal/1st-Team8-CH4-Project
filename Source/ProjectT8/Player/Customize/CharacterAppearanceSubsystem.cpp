#include "Player/Customize/CharacterAppearanceSubsystem.h"
#include "CharacterAppearanceSaveGame.h"
#include "Kismet/GameplayStatics.h"

const FString SaveSlot = TEXT("CharacterAppearanceSlot");

void UCharacterAppearanceSubsystem::SaveAppearance()
{
	UCharacterAppearanceSaveGame* SaveObj = Cast<UCharacterAppearanceSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UCharacterAppearanceSaveGame::StaticClass()));

	if (SaveObj)
	{	
		SaveObj->AppearanceData = CachedAppearanceData;
		UGameplayStatics::SaveGameToSlot(SaveObj, SaveSlot, 0);
		UE_LOG(LogTemp, Warning, TEXT("[Subsystem] 외형 정보 저장 완료!"));
	}
}

// 지윤님 확인!
// auto* Subsystem = GetGameInstance()->GetSubsystem<UCharacterAppearanceSubsystem>();
// Subsystem->CachedAppearanceData.TopID = "Top_03";
// Subsystem->SaveAppearance();

void UCharacterAppearanceSubsystem::LoadAppearance()
{	
	if (UGameplayStatics::DoesSaveGameExist(SaveSlot, 0))
	{
		UCharacterAppearanceSaveGame* Loaded = Cast<UCharacterAppearanceSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SaveSlot, 0));
	
		if (Loaded)
		{
			CachedAppearanceData = Loaded->AppearanceData;
			UE_LOG(LogTemp, Warning, TEXT("[Subsystem] 외형 정보 불러오기 완료!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Subsystem] 저장된 외형 정보 없음!"));
	}
}
