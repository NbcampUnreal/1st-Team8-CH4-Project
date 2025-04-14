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
	auto GetRandomID = [](const FString& Prefix, int32 Min, int32 Max) -> FString {
		int32 RandomNum = FMath::RandRange(Min, Max);
		return FString::Printf(TEXT("%s_%02d"), *Prefix, RandomNum);
	};

	const FString DefaultHeadID = GetRandomID("Head", 1, 3);
	const FString DefaultAccessoryID = GetRandomID("Accessory", 1, 7);
	const FString DefaultGlovesID = GetRandomID("Gloves", 1, 2);
	const FString DefaultTopID = GetRandomID("Top", 1, 5);
	const FString DefaultBottomID = GetRandomID("Bottom", 1, 3);
	const FString DefaultShoesID = GetRandomID("Shoes", 1, 3);

	if (UGameplayStatics::DoesSaveGameExist(SaveSlot, 0))
	{
		UCharacterAppearanceSaveGame* Loaded = Cast<UCharacterAppearanceSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SaveSlot, 0));
	
		if (Loaded)
		{
			CachedAppearanceData = Loaded->AppearanceData;
			
			// 저장된 데이터가 있어도 빈 값이 있다면 랜덤 기본값으로 설정
			if (CachedAppearanceData.HeadID.IsEmpty()) CachedAppearanceData.HeadID = DefaultHeadID;
			if (CachedAppearanceData.AccessoryID.IsEmpty()) CachedAppearanceData.AccessoryID = DefaultAccessoryID;
			if (CachedAppearanceData.GlovesID.IsEmpty()) CachedAppearanceData.GlovesID = DefaultGlovesID;
			if (CachedAppearanceData.TopID.IsEmpty()) CachedAppearanceData.TopID = DefaultTopID;
			if (CachedAppearanceData.BottomID.IsEmpty()) CachedAppearanceData.BottomID = DefaultBottomID;
			if (CachedAppearanceData.ShoesID.IsEmpty()) CachedAppearanceData.ShoesID = DefaultShoesID;

			UE_LOG(LogTemp, Warning, TEXT("[Subsystem] 외형 정보 불러오기 완료!"));
		}
	}
	else
	{
		CachedAppearanceData.HeadID = DefaultHeadID;
		CachedAppearanceData.AccessoryID = DefaultAccessoryID;
		CachedAppearanceData.GlovesID = DefaultGlovesID;
		CachedAppearanceData.TopID = DefaultTopID;
		CachedAppearanceData.BottomID = DefaultBottomID;
		CachedAppearanceData.ShoesID = DefaultShoesID;
		UE_LOG(LogTemp, Warning, TEXT("[Subsystem] 저장된 외형 정보 없음! 기본값 설정"));
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