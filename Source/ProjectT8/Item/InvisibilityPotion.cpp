#include "Item/InvisibilityPotion.h"

AInvisibilityPotion::AInvisibilityPotion()
{
	ItemName = "InvisibilityPotion";
}

void AInvisibilityPotion::Use(ACharacterBase* Player)
{
    //if (Player)
    //{
    //    Player->SetInvisibility(true, Duration); // player쪽에 투명화 로직 추가 SetInvisibility() 사용
    //    SetInvisibility(false);                 // 원래 상태
    //    Destroy();
    //}
}