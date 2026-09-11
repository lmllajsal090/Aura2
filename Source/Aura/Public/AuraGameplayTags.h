// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * 
 */
struct FAuraGameplayTags{

public:
	
	const static FAuraGameplayTags& Get(){return GameplayTags;}
	static void InitializeNativeGameplayTags();
private:
	const static FAuraGameplayTags GameplayTags;
};

