#include "T8GameMode.h"
#include "Player/CharacterBase.h"
#include "GameFramework/Common/T8PlayerState.h"
#include "GameFramework/Character.h"

AT8GameMode::AT8GameMode()
{	
	DefaultPawnClass = ACharacterBase::StaticClass();
}

void AT8GameMode::NotifyPlayerDeath_Implementation(ACharacter* DeadCharacter)
{
	// 여기에서 게임 로직에 따른 죽음 처리를 구현
	// 예: 점수 계산, 게임 종료 조건 체크 등
	
	// CharacterBase로 캐스팅이 필요한 경우
	if (ACharacterBase* BaseCharacter = Cast<ACharacterBase>(DeadCharacter))
	{
		// CharacterBase 특정 기능 사용
	}
}
