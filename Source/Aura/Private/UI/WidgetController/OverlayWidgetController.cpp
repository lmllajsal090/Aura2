// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
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
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChange.Broadcast(Data.NewValue);
			}
		);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChange.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChange.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChange.Broadcast(Data.NewValue);
			}
		);
	
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
                	[this](const FGameplayTagContainer& AssetTags)
                	{
                		for (const FGameplayTag& Tag : AssetTags)
                		{
							// For example, say that Tag = Message.HealthPotion
							// "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False
							FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
							if (Tag.MatchesTag(MessageTag))
							{
								const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);

								if (Row == nullptr)
								{
									UE_LOG(LogTemp, Error, TEXT("Message row not found: %s"), *Tag.ToString());
									continue;
								}
								
								UE_LOG(LogTemp, Warning, TEXT("Broadcast UI message: %s"), *Row->Message.ToString());


								MessageWidgetRowDelegate.Broadcast(*Row);
							}
						}
						
                	}
                	);
}



