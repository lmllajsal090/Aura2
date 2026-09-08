// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxMana.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"


UMMC_MaxMana::UMMC_MaxMana()
{
	IntelligenceDef.AttributeToCapture=UAuraAttributeSet::GetIntelligenceAttribute();
	IntelligenceDef.AttributeSource=EGameplayEffectAttributeCaptureSource::Target;
	IntelligenceDef.bSnapshot=false;
	
	RelevantAttributesToCapture.Add(IntelligenceDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags= Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags= Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	float Intelligence=0.f;
	GetCapturedAttributeMagnitude(IntelligenceDef,Spec,EvaluationParameters,Intelligence);
	Intelligence=FMath::Max(0.f,Intelligence);
	
	int32 PlayerLevel=1;
	ICombatInterface* CombatInterface=Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	if(CombatInterface)
	{
		PlayerLevel=CombatInterface->GetPlayerLevel();
	}
	
	return 2.5*Intelligence+10.f*PlayerLevel+50.f;
}
