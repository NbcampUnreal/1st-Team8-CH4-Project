#include "Multi/Widget/UserSlot.h"
#include "AdvancedSteamFriendsLibrary.h"
#include "Engine/TextureRenderTarget2D.h"

void UUserSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (CharacterRenderTargetToMaterial)
	{
		SetCharacterImage(CharacterRenderTargetToMaterial);
	}

	SetUserPersonaName();
}

void UUserSlot::SetCharacterName(const FText& NewText)
{
	if (CharacterName)
	{
		CharacterName->SetText(NewText);
	}
}

void UUserSlot::SetCharacterImage(UMaterialInterface* NewMaterial)
{
	if (CharacterImage && NewMaterial)
	{
		// 두 번째 인자는 이미지를 새 Material에 맞게 크기를 조정할지 여부를 나타냅니다.
		CharacterImage->SetBrushFromMaterial(NewMaterial);
	}
}

// 개념적 예시 (표준 OSS 사용, 단순화됨)
void UUserSlot::SetUserPersonaName() // 함수 이름 변경
{
	FString PersonaName = TEXT("Unknown User");
	IOnlineSubsystem* SteamSubsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (SteamSubsystem)
	{
		IOnlineIdentityPtr Identity = SteamSubsystem->GetIdentityInterface();
		if (Identity.IsValid() && Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn) // 로그인 상태 확인 추가
		{
			// 캐시된 닉네임 우선 사용
			PersonaName = Identity->GetPlayerNickname(0);
			if (PersonaName.IsEmpty()) {
				PersonaName = TEXT("Steam User"); // 캐시 비었을 때 기본값
				// TODO: 여기서 QueryUserInfo를 호출하여 비동기적으로
				// 최신 닉네임을 가져오고, 완료 시 UI를 업데이트하는 로직 추가 필요
			}
		}
		else {
			PersonaName = TEXT("Error: Not Logged In or No Identity");
		}
	}
	else {
		PersonaName = TEXT("Error: Steam Subsystem Unavailable");
	}

	if (CharacterName)
	{
		CharacterName->SetText(FText::FromString(PersonaName));
	}
}