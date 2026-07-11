// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraabilitySystemComponent.h"

AAuraPlayerState::AAuraPlayerState()
{
	NetUpdateFrequency =100.f;
	
	AbilitySystemComponent=CreateDefaultSubobject<UAuraabilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	
	AttributeSet=CreateDefaultSubobject<UAttributeSet>("AttributeSet");
}
