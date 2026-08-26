// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AuraAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraabilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"


UAuraAttributeSet::UAuraAttributeSet()
{
	InitMaxHealth(100.f);
	InitHealth(50.f);
	InitMaxMana(50.f);
	InitMana(50.f);
	
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Health,COND_None,REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxHealth,COND_None,REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Mana,COND_None,REPNOTIFY_Always)
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxMana,COND_None,REPNOTIFY_Always)
}

//用来做数值夹紧，确认数值的范围
void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute==GetHealthAttribute())
	{
		NewValue=FMath::Clamp(NewValue,0.f,GetMaxHealth());
		UE_LOG(LogTemp,Warning,TEXT("Health:%f"),NewValue);
	}

	if (Attribute==GetManaAttribute())
	{
		NewValue=FMath::Clamp(NewValue,0.f,GetMaxMana());
		UE_LOG(LogTemp,Warning,TEXT("Mana:%f"),NewValue);
	}

}

void UAuraAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	// Use Props here for damage, knockback, floating combat text, death checks, etc.
}

void UAuraAttributeSet::SetEffectProperties(
	const FGameplayEffectModCallbackData& Data,
	FEffectProperties& Props) const
{
	Props = FEffectProperties{};

	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC =
		Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC))
	{
		Props.SourceAvatarActor = Props.SourceASC->GetAvatarActor();
	}

	if (IsValid(Props.SourceAvatarActor))
	{
		Props.SourceCharacter =
			Cast<ACharacter>(Props.SourceAvatarActor);

		if (const APawn* SourcePawn =
			Cast<APawn>(Props.SourceAvatarActor))
		{
			Props.SourceController = SourcePawn->GetController();
		}
	}

	Props.TargetASC = &Data.Target;
	Props.TargetAvatarActor = Props.TargetASC->GetAvatarActor();

	if (IsValid(Props.TargetAvatarActor))
	{
		Props.TargetCharacter =
			Cast<ACharacter>(Props.TargetAvatarActor);

		if (const APawn* TargetPawn =
			Cast<APawn>(Props.TargetAvatarActor))
		{
			Props.TargetController = TargetPawn->GetController();
		}
	}
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Health,OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxHealth,OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Mana,OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxMana,OldMaxMana);
}
