// Copyright © 2021 Tanzq. All rights reserved.


#include "AnimNotify_GroundedEntryState.h"

#include "ALSVLearnDemo/Interfaces/ALS_Animation.h"

FString UAnimNotify_GroundedEntryState::GetNotifyName_Implementation() const
{
	const UEnum* CompileModeEnum = StaticEnum<EGroundedEntryState>();

	return FString::Printf(TEXT("Grounded Entry State: %s"), *CompileModeEnum->GetDisplayNameTextByValue(
		static_cast<uint8>(GroundedEntryState)).ToString());
}

void UAnimNotify_GroundedEntryState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	IALS_Animation* AnimationInterface = Cast<IALS_Animation>(MeshComp->GetAnimInstance());
	if (!AnimationInterface) return;

	AnimationInterface->Execute_API_SetGroundedEntryState(MeshComp->GetAnimInstance(), GroundedEntryState);
}
