#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/Base/ScreenBaseWidget.h"
#include "UI/TeamStatusBar.h"
#include "PlayingScreenWidget.generated.h"

class UTextBlock;
class UTeamStatusBar;
class ACharacter;

UCLASS()
class PROJECTT8_API UPlayingScreenWidget : public UScreenBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// 캐릭터 죽음 처리
	UFUNCTION()
	void OnCharacterDeath(ACharacter* DeadCharacter);

	// 플레이어 상태 업데이트
	void UpdatePlayerStatus(const FString& PlayerName, bool bIsAlive);

protected:
	UPROPERTY(meta=(BindWidget))
	class UTeamStatusBar* TeamStatusBar;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TeamModeText;

	// 플레이어 상태 데이터 저장
	UPROPERTY()
	TMap<FString, FPlayerStatusUIData> PlayerStatusMap;
};
