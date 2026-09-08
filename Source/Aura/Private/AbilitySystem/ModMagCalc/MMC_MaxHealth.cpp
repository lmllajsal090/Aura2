// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	//捕获谁＋捕获对象＋是否快照捕获＋注册捕获规则
	VigorDef.AttributeToCapture=UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource=EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot=false;
	
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//获取来源和目标身上的标签
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.f);
	
	int32 PlayerLevel = 1;
	ICombatInterface* CombatInterface =Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	if(CombatInterface)
	{
		PlayerLevel = CombatInterface->GetPlayerLevel();
	}
	return Vigor*2.5+PlayerLevel*10.f+80.f;
}
