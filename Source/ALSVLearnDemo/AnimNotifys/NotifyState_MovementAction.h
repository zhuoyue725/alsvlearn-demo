// Copyright © 2021 Tanzq. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ALSVLearnDemo/Data/Enums/ALS_MovementAction.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NotifyState_MovementAction.generated.h"

/**
 * 
 */
UCLASS()
class ALSVLEARNDEMO_API UNotifyState_MovementAction : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EALS_MovementAction MovementAction;

	virtual FString GetNotifyName_Implementation() const override;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
