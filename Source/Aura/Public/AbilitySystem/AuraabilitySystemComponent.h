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
	
	void AbilityActorInfoSet();

	FEffectAppliedDelegate EffectAssetTags;
protected:
	
	void EffectApplied( UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveHandle );
};
