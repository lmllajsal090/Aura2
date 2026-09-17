// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraabilitySystemComponent.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAppliedDelegate,const FGameplayTagContainer&/*AssetTags*/);

UCLASS()
class AURA_API UAuraabilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public: 
	
	
	void AbilityActorInfoSet() ;
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupGameplayAbilities);
	
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	FEffectAppliedDelegate EffectAssetTags;
protected:
	UFUNCTION(Client,Reliable)
	void ClientEffectApplied( UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveHandle );
	

};
