#include "Player/Customize/CharacterAppearanceSubsystem.h"
#include "CharacterAppearanceSaveGame.h"
#include "Data/CostumeItemDataRow.h"
#include "GameFramework/Common/T8PlayerState.h"
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

		FString DynamicSlot = GetDynamicSaveSlot();
		UGameplayStatics::SaveGameToSlot(SaveObj, DynamicSlot, 0);
		UE_LOG(LogTemp, Warning, TEXT("[Subsystem] 외형 정보 저장 완료! (%s)"), *DynamicSlot);
	}
}

void UCharacterAppearanceSubsystem::LoadAppearance()
{	
	FString DynamicSlot = GetDynamicSaveSlot();

	if (UGameplayStatics::DoesSaveGameExist(DynamicSlot, 0))
	{
		UCharacterAppearanceSaveGame* Loaded = Cast<UCharacterAppearanceSaveGame>(
			UGameplayStatics::LoadGameFromSlot(DynamicSlot, 0));
	
		if (Loaded)
		{
			CachedAppearanceData = Loaded->AppearanceData;
			UE_LOG(LogTemp, Warning, TEXT("[Subsystem] 저장된 외형 정보를 불러왔습니다! (%s)"), *DynamicSlot);
			return;
		}
	}

	// 저장된 데이터가 없을 경우에만 랜덤 값 생성
	UE_LOG(LogTemp, Warning, TEXT("[Subsystem] 저장된 외형 정보 없음! 기본값 설정 (%s)"), *DynamicSlot);
	
	auto GetRandomID = [](const FString& Prefix, int32 Min, int32 Max) -> FString {
		int32 RandomNum = FMath::RandRange(Min, Max);
		return FString::Printf(TEXT("%s_%02d"), *Prefix, RandomNum);
	};

	CachedAppearanceData.HeadID = GetRandomID("Head", 1, 3);
	CachedAppearanceData.AccessoryID = GetRandomID("Accessory", 1, 7);
	CachedAppearanceData.GlovesID = GetRandomID("Gloves", 1, 2);
	CachedAppearanceData.TopID = GetRandomID("Top", 1, 5);
	CachedAppearanceData.BottomID = GetRandomID("Bottom", 1, 3);
	CachedAppearanceData.ShoesID = GetRandomID("Shoes", 1, 3);

	// 새로 생성된 랜덤 값을 저장
	SaveAppearance();
}

FString UCharacterAppearanceSubsystem::GetDynamicSaveSlot() const
{
	FString SlotPrefix = SaveSlot;

	// 가장 먼저 PlayerController -> PlayerState 기반 ID
	if (const APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (AT8PlayerState* PS = Cast<AT8PlayerState>(PC->PlayerState))
		{
			// 가장 좋은 방식: PlayerName 기반
			return FString::Printf(TEXT("%s_%s"), *SlotPrefix, *PS->GetPlayerName());
		}
	}

	// 아니면 FPlatformMisc 기준 fallback
	FString DeviceId = FPlatformMisc::GetDeviceId();
	return FString::Printf(TEXT("%s_%s"), *SlotPrefix, *DeviceId);
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