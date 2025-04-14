#include "Player/Customize/CharacterAppearanceSubsystem.h"
#include "CharacterAppearanceSaveGame.h"
#include "Data/CostumeItemDataRow.h"
#include "Kismet/GameplayStatics.h"

const FString SaveSlot = TEXT("CharacterAppearanceSlot");

UCharacterAppearanceSubsystem::UCharacterAppearanceSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_ItemTable(TEXT("/Game/DataTables/DT_CostumeItemTable.DT_CostumeItemTable"));

	if (DT_ItemTable.Succeeded())
	{
		ItemTable = DT_ItemTable.Object;
		UE_LOG(LogTemp, Warning, TEXT("아이템 테이블 불러오기 성공!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("아이템 테이블 불러오기 실패!"));
	}
}

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
		CachedAppearanceData.HeadID = "Head_01";
		CachedAppearanceData.AccessoryID = "Accessory_01";
		CachedAppearanceData.GlovesID = "Gloves_01";
		CachedAppearanceData.TopID = "Top_01";
		CachedAppearanceData.BottomID = "Bottom_01";
		CachedAppearanceData.ShoesID = "Shoes_01";
		UE_LOG(LogTemp, Warning, TEXT("[Subsystem] 저장된 외형 정보 없음!"));
	}
}

USkeletalMesh* UCharacterAppearanceSubsystem::GetCostumeMeshByID(const FString& ItemID)
{
	if (!ItemTable) return nullptr;

	const FCostumeItemDataRow* Row = ItemTable->FindRow<FCostumeItemDataRow>(*ItemID, TEXT(""));
	UE_LOG(LogTemp, Warning, TEXT("ItemID: %s"), *ItemID);
	if (Row)
	{
		return Row->SkeletalMesh;
	}

	return nullptr;
}