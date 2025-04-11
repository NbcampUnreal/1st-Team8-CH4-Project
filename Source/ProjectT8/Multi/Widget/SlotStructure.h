#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotStructure.generated.h"

class UUniformGridPanel;
class UButton;
class ALobbyGameState;

UCLASS()
class PROJECTT8_API USlotStructure : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    // 블루프린트 에디터에서 지정할 UniformGridPanel (SlotGridPanel 위젯)
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* SlotGridPanel;

    // 동적으로 생성한 슬롯 버튼들을 저장할 배열 (Transient)
    UPROPERTY(Transient)
    TArray<UButton*> SlotButtons;

    // 슬롯 버튼들을 생성하여 그리드에 추가하는 함수
    void SetupSlotButtons();

    // 공통으로 호출되는 함수, SlotIndex에 해당하는 슬롯으로 이동 요청
    void OnSlotButtonClicked(int32 SlotIndex);

    // 각 슬롯 버튼 전용 클릭 핸들러 (동적 delegate 바인딩을 위해 각각 UFUNCTION으로 선언)
    UFUNCTION()
    void OnSlotButton0Clicked();

    UFUNCTION()
    void OnSlotButton1Clicked();

    UFUNCTION()
    void OnSlotButton2Clicked();

    UFUNCTION()
    void OnSlotButton3Clicked();

    UFUNCTION()
    void OnSlotButton4Clicked();

    UFUNCTION()
    void OnSlotButton5Clicked();

    UFUNCTION()
    void OnSlotButton6Clicked();

    UFUNCTION()
    void OnSlotButton7Clicked();

    // 현재 로비 GameState를 얻는 헬퍼 함수
    ALobbyGameState* GetLobbyGameState() const;
};
