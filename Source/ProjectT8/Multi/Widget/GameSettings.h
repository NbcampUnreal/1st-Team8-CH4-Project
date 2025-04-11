

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameSettings.generated.h"

class UTextBlock;
class UButton;
class ALobbyGameState;

UCLASS()
class PROJECTT8_API UGameSettings : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;

protected:
    /** 팀 모드 표시 텍스트 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TeamModeText;
    /** 팀 모드 변경 버튼 (호스트 전용) */
    UPROPERTY(meta = (BindWidget))
    UButton* TeamModeButton;
    /** 선택 맵 표시 텍스트 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* MapText;
    /** 맵 변경 버튼 (호스트 전용) */
    UPROPERTY(meta = (BindWidget))
    UButton* MapButton;

    /** 팀 모드 변경 버튼 클릭 핸들러 */
    UFUNCTION()
    void OnTeamModeButtonClicked();
    /** 맵 변경 버튼 클릭 핸들러 */
    UFUNCTION()
    void OnMapButtonClicked();

    /** 현재 로비 GameState (SlotStructure) 캐시 */
    ALobbyGameState* LobbyState;
};