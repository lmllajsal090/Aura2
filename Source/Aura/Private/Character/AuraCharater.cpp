// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharater.h"

#include "GameFramework/CharacterMovementComponent.h"

AAuraCharater::AAuraCharater()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	bUseControllerRotationPitch=false;
	bUseControllerRotationRoll=false;
	bUseControllerRotationYaw=false;
}
 