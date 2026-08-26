// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgerController/OverlayWidgetController.h"

#include "AbilitySystem/AuraabilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UOverlayWidgetController::BroadcastInitiaValue()
{
	UAuraAttributeSet* AuraAttributeSet=CastChecked<UAuraAttributeSet>(AttributeSet);
	
	OnMaxHealthChange.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnHealthChange.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxManaChange.Broadcast(AuraAttributeSet->GetMaxMana());
	OnManaChange.Broadcast(AuraAttributeSet->GetMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet=CastChecked<UAuraAttributeSet>(AttributeSet);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetHealthAttribute()).AddUObject(this,&UOverlayWidgetController::HealthChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxHealthAttribute()).AddUObject(this,&UOverlayWidgetController::MaxHealthChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetManaAttribute()).AddUObject(this,&UOverlayWidgetController::ManaChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxManaAttribute()).AddUObject(this,&UOverlayWidgetController::MaxManaChanged);
	
	Cast<UAuraabilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
                	[this](const FGameplayTagContainer& AssetTags)
                	{
                		for (const FGameplayTag& Tag : AssetTags)
                		{
                			const FString Msg=FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
                			GEngine->AddOnScreenDebugMessage(-1, 8.f,FColor::Blue, Msg);

                			FUIWidgetRow * Row=GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable,Tag);

                		}
                	}	
                	);
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChange.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChange.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChange.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChange.Broadcast(Data.NewValue);
}
