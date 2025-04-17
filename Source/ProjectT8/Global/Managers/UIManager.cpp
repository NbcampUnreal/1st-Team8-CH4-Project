#include "Global/Managers/UIManager.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/Common/T8GameInstance.h"
#include "Global/Datas/PhaseInfoDataAsset.h"
#include "UI/Datas/PhaseInfoItem.h"
#include "UI/Widgets/Base/ScreenBaseWidget.h"


void UUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Loading DataAsset
    if (const UT8GameInstance* GI = Cast<UT8GameInstance>(GetGameInstance()))
    {
        PhaseInfoDataAsset = GI->PhaseInfoDataAsset;
    }
    if (!PhaseInfoDataAsset)
    {
        return;
    }

    // Initializing PhaseWidgetMap
    for (UPhaseInfoItem* Item : PhaseInfoDataAsset->PhaseInfos)
    {
        if (Item && Item->WidgetClass)
        {
            PhaseWidgetMap.Add(Item->Phase, Item->WidgetClass);
        }
    }
}

void UUIManager::ShowUI(EGamePhase Phase)
{
    // Hide Current UI
    HideCurUI(); 

    // Searching for New UI
    if (TSubclassOf<UUserWidget>* WidgetClassPtr = PhaseWidgetMap.Find(Phase))
    {
        UWorld* World = GetWorld();
        if (!World || !*WidgetClassPtr)
        {
            return;
        }

        APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
        if (!PC)
        {
            return;
        }

        UUserWidget* NewWidget = CreateWidget<UUserWidget>(PC, *WidgetClassPtr);

        if (NewWidget)
        {
            // Show New UI
            CurWidget = NewWidget;
            CurWidget->AddToViewport();
            //PC->SetInputMode(FInputModeUIOnly());
            //PC->SetShowMouseCursor(true);
            
            if (UScreenBaseWidget* CurScreen = Cast<UScreenBaseWidget>(CurWidget))
            {
                CurScreen->OnScreenActivated();
            }
        }
    }
}

void UUIManager::HideCurUI()
{
    if (!CurWidget)
    {
        return;
    }

    if (UScreenBaseWidget* CurScreen = Cast<UScreenBaseWidget>(CurWidget))
    {
        // ScreenBaseWidget�� ��ӹ��� Screen�� ���, FadeOut ���� ��� �� Deactivate
        if (!CurScreen->FadeOutFinishedDelegate.IsBound())
        {
            FWidgetAnimationDynamicEvent ExternalFadeOutDelegate;
            ExternalFadeOutDelegate.BindDynamic(CurScreen, &UScreenBaseWidget::HandleFadeOutFinished);
            CurScreen->FadeOutFinishedDelegate = ExternalFadeOutDelegate;
        }

        CurScreen->OnScreenDeactivated();
    }
    else
    {
        // ScreenBaseWidget�� ��ӹ��� Screen�� �ƴ� ���, �׳� ����
        CurWidget->RemoveFromParent();
        CurWidget = nullptr;
    }
}

void UUIManager::NotifyScreenRemoved(UUserWidget* Widget)
{
    if (Widget == CurWidget)
    {
        CurWidget = nullptr;
    }
}