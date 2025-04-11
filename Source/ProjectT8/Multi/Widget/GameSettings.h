

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
    /** �� ��� ǥ�� �ؽ�Ʈ */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TeamModeText;
    /** �� ��� ���� ��ư (ȣ��Ʈ ����) */
    UPROPERTY(meta = (BindWidget))
    UButton* TeamModeButton;
    /** ���� �� ǥ�� �ؽ�Ʈ */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* MapText;
    /** �� ���� ��ư (ȣ��Ʈ ����) */
    UPROPERTY(meta = (BindWidget))
    UButton* MapButton;

    /** �� ��� ���� ��ư Ŭ�� �ڵ鷯 */
    UFUNCTION()
    void OnTeamModeButtonClicked();
    /** �� ���� ��ư Ŭ�� �ڵ鷯 */
    UFUNCTION()
    void OnMapButtonClicked();

    /** ���� �κ� GameState (SlotStructure) ĳ�� */
    ALobbyGameState* LobbyState;
};