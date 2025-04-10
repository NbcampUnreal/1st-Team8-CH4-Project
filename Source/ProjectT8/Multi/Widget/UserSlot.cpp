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
		// �� ��° ���ڴ� �̹����� �� Material�� �°� ũ�⸦ �������� ���θ� ��Ÿ���ϴ�.
		CharacterImage->SetBrushFromMaterial(NewMaterial);
	}
}

// ������ ���� (ǥ�� OSS ���, �ܼ�ȭ��)
void UUserSlot::SetUserPersonaName() // �Լ� �̸� ����
{
	FString PersonaName = TEXT("Unknown User");
	IOnlineSubsystem* SteamSubsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (SteamSubsystem)
	{
		IOnlineIdentityPtr Identity = SteamSubsystem->GetIdentityInterface();
		if (Identity.IsValid() && Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn) // �α��� ���� Ȯ�� �߰�
		{
			// ĳ�õ� �г��� �켱 ���
			PersonaName = Identity->GetPlayerNickname(0);
			if (PersonaName.IsEmpty()) {
				PersonaName = TEXT("Steam User"); // ĳ�� ����� �� �⺻��
				// TODO: ���⼭ QueryUserInfo�� ȣ���Ͽ� �񵿱�������
				// �ֽ� �г����� ��������, �Ϸ� �� UI�� ������Ʈ�ϴ� ���� �߰� �ʿ�
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