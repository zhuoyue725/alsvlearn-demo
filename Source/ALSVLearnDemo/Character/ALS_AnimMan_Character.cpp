// Fill out your copyright notice in the Description page of Project Settings.


#include "ALS_AnimMan_Character.h"

FTransform AALS_AnimMan_Character::Get3PPivotTarget_Implementation()
{
	// 使用具体的两个插槽计算角色的中心点更加精准。比如说胶囊体不会缩小，当角色蹲下的时候，我们获取人物的位置的时候并不会发生改变，但如果是根据骨骼位置然后求中心点的话，这个位置就会跟着发生改变。
	const FVector Location = (GetMesh()->GetSocketLocation(TEXT("head")) + GetMesh()->GetSocketLocation(TEXT("root")))
		/ 2.f;
	
	return FTransform(GetActorRotation(), Location, FVector(1.f, 1.f, 1.f));
}

UAnimMontage* AALS_AnimMan_Character::GetRollAnimation()
{
	// TODO Overlay State
	return LandRollDefault;
}
