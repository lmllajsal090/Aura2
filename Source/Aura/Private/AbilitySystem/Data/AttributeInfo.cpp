// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoFromGameplayTag(FGameplayTag AttributeTag, bool bLogErrorIfNotFound) const
{
	for (const FAuraAttributeInfo &Info :AttributeInformation)
	{
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return Info;
		}
		
	}
	if (bLogErrorIfNotFound)
	{
     		UE_LOG(LogTemp,Error,TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]."),*AttributeTag.ToString(),*GetNameSafe(this))	
	}
	return FAuraAttributeInfo();
}
