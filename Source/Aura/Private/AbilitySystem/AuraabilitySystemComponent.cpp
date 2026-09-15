// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraabilitySystemComponent.h"
#include "GameplayTagContainer.h" 
#include "AbilitySystem/Ability/AuraGameplayAbility.h"

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
	for (const TSubclassOf<UGameplayAbility> Ability : StartupGameplayAbilities)
	{
		
		FGameplayAbilitySpec Spec=FGameplayAbilitySpec(Ability,1);
		if (const UAuraGameplayAbility* AuraAbility=Cast<UAuraGameplayAbility>(Spec.Ability))
		{
			Spec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			GiveAbility(Spec);
		}
		
	}
}

void UAuraabilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	
}

void UAuraabilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	for (auto& AbilitySpec:GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraabilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	for (auto& AbilitySpec:GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAuraabilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	
	EffectAssetTags.Broadcast(TagContainer);
	
	
}
