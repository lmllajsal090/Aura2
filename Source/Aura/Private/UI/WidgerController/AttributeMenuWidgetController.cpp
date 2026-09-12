// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgerController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"

void UAttributeMenuWidgetController::BroadcastInitiaValue()
{
	UAuraAttributeSet* AuraAttributeSet=CastChecked<UAuraAttributeSet>(AttributeSet);
	
	
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
}
