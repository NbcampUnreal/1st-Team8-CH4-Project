#include "Global/Managers/UIManager.h"
#include "Blueprint/UserWidget.h"

#include "GameFramework/Common/T8GameInstance.h"
#include "Global/Datas/PhaseWidgetDataAsset.h"
#include "UI/Datas/PhaseWidgetItem.h"

void UUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // PhaseWidgetMap Init
    if (const UT8GameInstance* GI = Cast<UT8GameInstance>(GetGameInstance()))
    {
        WidgetDataAsset = GI->WidgetDataAsset;
    }
    if (!WidgetDataAsset) return;

    for (UPhaseWidgetItem* Item : WidgetDataAsset->PhaseWidgets)
    {
        if (Item && Item->WidgetClass)
        {
            PhaseWidgetMap.Add(Item->Phase, Item->WidgetClass);
        }
    }
}

void UUIManager::ShowUI(EGamePhase Phase)
{
    
}

void UUIManager::HideCurUI()
{

}