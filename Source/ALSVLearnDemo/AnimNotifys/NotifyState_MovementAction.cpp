// Copyright © 2021 Tanzq. All rights reserved.


#include "NotifyState_MovementAction.h"

#include "ALSVLearnDemo/Interfaces/ALS_Character.h"

FString UNotifyState_MovementAction::GetNotifyName_Implementation() const
{
	const UEnum* CompileModeEnum = StaticEnum<EALS_MovementAction>();

	return FString::Printf(TEXT("Movement Action: %s"), *CompileModeEnum->GetDisplayNameTextByValue(
		                       static_cast<uint8>(MovementAction)).ToString());
}

void UNotifyState_MovementAction::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{
	const IALS_Character* ICharacter = Cast<IALS_Character>(MeshComp->GetOwner());
	if (!ICharacter) return;
	
	ICharacter->Execute_API_SetMovementAction(MeshComp->GetOwner(), MovementAction);
}

void UNotifyState_MovementAction::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	const auto Owner = MeshComp->GetOwner();
	const IALS_Character* ICharacter = Cast<IALS_Character>(Owner);
	if (!ICharacter) return;

	EALS_MovementAction CurrentMovementAction;
	EALS_MovementState MovementState;
	EALS_Gait Gait;
	EALS_Stance Stance;
	EALS_RotationMode RotationMode;
	
	ICharacter->Execute_API_GetCurrentStates(Owner,  MovementState, CurrentMovementAction, Gait, Stance, RotationMode);
	if (MovementAction == CurrentMovementAction)
	{
		ICharacter->Execute_API_SetMovementAction(Owner, EALS_MovementAction::EAMs_None);
	}
}
