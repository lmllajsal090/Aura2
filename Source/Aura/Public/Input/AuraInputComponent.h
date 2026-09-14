// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	template<class UserClass,typename PressedFuncType,typename ReleasedFuncType,typename HeldFuncType>
	void  BindAbilityActions(UAuraInputConfig* InputConfig, UserClass* UserObject, PressedFuncType PressedFunc,ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);

};

template<class UserClass,typename PressedFuncType,typename ReleasedFuncType,typename HeldFuncType>
	void UAuraInputComponent::BindAbilityActions(UAuraInputConfig* InputConfig, UserClass* UserObject, PressedFuncType PressedFunc,
							ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);
	
	for (const FAuraInputAction& Action:InputConfig->InputActions)
	{
		if (Action.InputAction&&Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Started,UserObject,PressedFunc,Action.InputTag);
			}
			
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Completed,UserObject,ReleasedFunc,Action.InputTag);
			}
			
			if (PressedFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Triggered,UserObject,PressedFunc,Action.InputTag);
			}
		}
	}
}
