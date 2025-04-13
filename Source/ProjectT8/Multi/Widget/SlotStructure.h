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
    // �������Ʈ �����Ϳ��� ������ UniformGridPanel (SlotGridPanel ����)
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* SlotGridPanel;

    // �������� ������ ���� ��ư���� ������ �迭 (Transient)
    UPROPERTY(Transient)
    TArray<UButton*> SlotButtons;

    // ���� ��ư���� �����Ͽ� �׸��忡 �߰��ϴ� �Լ�
    void SetupSlotButtons();

    // �������� ȣ��Ǵ� �Լ�, SlotIndex�� �ش��ϴ� �������� �̵� ��û
    void OnSlotButtonClicked(int32 SlotIndex);

    // �� ���� ��ư ���� Ŭ�� �ڵ鷯 (���� delegate ���ε��� ���� ���� UFUNCTION���� ����)
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

    // ���� �κ� GameState�� ��� ���� �Լ�
    ALobbyGameState* GetLobbyGameState() const;
};
