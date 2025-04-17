#include "Global/Managers/UIManager.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/Common/T8GameInstance.h"
#include "Global/Datas/PhaseInfoDataAsset.h"
#include "UI/Datas/PhaseInfoItem.h"
#include "UI/Widgets/Base/ScreenBaseWidget.h"
#include "UI/Widgets/Screen/LoadingScreenWidget.h"


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
    
    for (UPhaseInfoItem* Item : PhaseInfoDataAsset->PhaseInfos)
    {
        // Initializing PhaseTargetNameMap
        if (Item/* && Item->MapName*/)
        {
            PhaseTargetNameMap.Add(Item->Phase, Item->MapName);
        }

        // Initializing PhaseWidgetMap
        if (Item && Item->WidgetClass)
        {
            PhaseWidgetMap.Add(Item->Phase, Item->WidgetClass);
        }
    }
}

void UUIManager::ShowUI(EGamePhase Phase)
{
    // Current Screen �����
    HideCurUI(); 

    // New Phase�� ���� New Screen �˻�
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

        // New Screen �׸���
        UUserWidget* NewWidget = CreateWidget<UUserWidget>(PC, *WidgetClassPtr);

        if (NewWidget)
        {
            CurWidget = NewWidget;
            CurWidget->AddToViewport();
            
            if (UScreenBaseWidget* CurScreen = Cast<UScreenBaseWidget>(CurWidget))
            {
                CurScreen->OnScreenActivated();
            }
        }

        // New Screen�� LoadingScreen�� ���: ProgressBar ���� ���
        if (ULoadingScreenWidget* LoadingScreen = Cast<ULoadingScreenWidget>(NewWidget))
        {
            // External Delegate <-> Handle Method
            FWidgetAnimationDynamicEvent ExternalProgressBarDelegate;
            ExternalProgressBarDelegate.BindDynamic(LoadingScreen, &ULoadingScreenWidget::HandleProgressBarFinished);
            
            // Internal Delegate <-> External Delegate
            LoadingScreen->ProgressBarFinishedDelegate = ExternalProgressBarDelegate;

            // Widget Animaation <-> Internal Delegate
            LoadingScreen->BindProgressBarFinished();

            // Now Play Animation!
            LoadingScreen->PlayAnimation(LoadingScreen->Anim_ProgressBar_Loading);
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
        // Current Screen�� ScreenBaseWidget�� ��ӹ��� ���: FadeOut ���� ���
        CurScreen->FadeOutFinishedDelegate.Unbind();

        FWidgetAnimationDynamicEvent ExternalFadeOutDelegate;
        ExternalFadeOutDelegate.BindDynamic(CurScreen, &UScreenBaseWidget::HandleFadeOutFinished);

        CurScreen->FadeOutFinishedDelegate = ExternalFadeOutDelegate;

        CurScreen->BindFadeOutFinished();

        CurScreen->OnScreenDeactivated();
    }
    else
    {
        // Current Screen�� ScreenBaseWidget�� ��ӹ��� �ʾ��� ���: �ٷ� ����
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

void UUIManager::OpenLevelForPhase(EGamePhase Phase)
{
    if (const FName* FoundMapName = PhaseTargetNameMap.Find(Phase))
    {
        UGameplayStatics::OpenLevel(this, *FoundMapName);
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("OpenLevel failed: No map found for Phase %d"), (int32)Phase);
}