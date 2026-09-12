// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/AuraAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgerController/AuraWidgetController.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC=UGameplayStatics::GetPlayerController(WorldContextObject,0))
	{
		if (AAuraHUD* AuraHud=Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS=PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC=PS->GetAbilitySystemComponent();
			UAttributeSet* AC=PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AC);
			return AuraHud->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}
