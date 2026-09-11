// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagContainer.h"

/**
 * 
 */
struct FAuraGameplayTags{

public:
	FGameplayTag Attributes_Secondary_Armor;
	
	const static FAuraGameplayTags& Get(){return GameplayTags;}
	static void InitializeNativeGameplayTags();
private:
	static FAuraGameplayTags GameplayTags;
};

