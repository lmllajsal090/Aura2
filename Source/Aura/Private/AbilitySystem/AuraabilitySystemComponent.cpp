// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraabilitySystemComponent.h"
#include "GameplayTagContainer.h" 

void UAuraabilitySystemComponent::AbilityActorInfoSet() 
{

	OnGameplayEffectAppliedDelegateToSelf.AddUObject(
        this,
        &UAuraabilitySystemComponent::EffectApplied
    );
}

void UAuraabilitySystemComponent::AddCharacterAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupGameplayAbilities)
{
	for (TSubclassOf<UGameplayAbility> Ability : StartupGameplayAbilities)
	{
	
		FGameplayAbilitySpec Spec=FGameplayAbilitySpec(Ability,1);
		GiveAbilityAndActivateOnce(Spec);
	}
}

void UAuraabilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	
	EffectAssetTags.Broadcast(TagContainer);
	
	
}
