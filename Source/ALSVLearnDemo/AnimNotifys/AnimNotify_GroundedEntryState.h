// Copyright © 2021 Tanzq. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ALSVLearnDemo/Data/Enums/GroundEntryState.h"
#include "AnimNotify_GroundedEntryState.generated.h"

/**
 * 
 */
UCLASS()
class ALSVLEARNDEMO_API UAnimNotify_GroundedEntryState : public UAnimNotify
{
	GENERATED_BODY()

public:
	/**
	 * @brief 进入地面的状态
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGroundedEntryState GroundedEntryState;

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
