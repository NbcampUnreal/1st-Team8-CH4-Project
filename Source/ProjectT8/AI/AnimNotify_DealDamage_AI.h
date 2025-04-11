#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_DealDamage_AI.generated.h"

UCLASS(meta = (DisplayName = "AI Deal Damage Notify"))
class PROJECTT8_API UAnimNotify_DealDamage_AI : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
