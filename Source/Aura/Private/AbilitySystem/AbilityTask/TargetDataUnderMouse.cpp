// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj= NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	const FGameplayAbilityActorInfo* ActorInfo =Ability->GetCurrentActorInfo();

	const bool bIsLocallyControlled = ActorInfo->IsLocallyControlled();
	if(bIsLocallyControlled)
	{
		SendMouseCursorData();
	}else
	{
		//TOOD:We are on the server,so listen for target data
	}

	FHitResult CursorHit;
	APlayerController* PlayerController = ActorInfo->PlayerController.Get();
	PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
	 FGameplayAbilityTargetData_SingleTargetHit* Data =new FGameplayAbilityTargetData_SingleTargetHit();
	 Data->HitResult = CursorHit;
	 FGameplayAbilityTargetDataHandle DataHandle;
	 DataHandle.Add(Data);
	
	 ValidData.Broadcast(DataHandle);
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	FGameplayAbilityTargetData_SingleTargetHit* Data =new FGameplayAbilityTargetData_SingleTargetHit();
	FHitResult CursorHit;
	APlayerController* PlayerController =Ability->GetCurrentActorInfo()->PlayerController.Get();
	PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
	
	Data->HitResult = CursorHit;
	FGameplayAbilityTargetDataHandle DataHandle;
	DataHandle.Add(Data);
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}


