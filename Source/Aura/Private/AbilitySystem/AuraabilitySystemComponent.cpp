// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraabilitySystemComponent.h"
#include "GameplayTagContainer.h" 

void UAuraabilitySystemComponent::AbilityActorInfoSet()
{

}

void UAuraabilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	
	EffectAssetTags.Broadcast(TagContainer);
	
	
}
