// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgerController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase,public IEnemyInterface
{
	GENERATED_BODY()
	
public:
	AAuraEnemy();
	/**Enemy Interface**/
	virtual void HighlightActor()  override;
	virtual void UnHighlightActor() override;
	
	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted=false;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	virtual int32 GetPlayerLevel() override;
protected:
	virtual void BeginPlay() override;
	virtual  void InitAbilityActorInfo() override;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Class Deafults")
	int32 Level=1;
	
	
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
};
