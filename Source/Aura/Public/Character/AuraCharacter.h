// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TimerHandle.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInterface;
class UPhysicsAsset;

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
	
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual int32 GetPlayerLevel() override;

protected:
	virtual void MulticastHandleDeath_Implementation() override;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Death")
	TObjectPtr<UPhysicsAsset> DeathPhysicsAsset;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Death")
	TArray<TObjectPtr<UMaterialInterface>> BodyDeathMaterials;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Death")
	TArray<TObjectPtr<UMaterialInterface>> WeaponDeathMaterials;

private:
	virtual void InitAbilityActorInfo() override;
	void UpdateDeathDissolve();

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DeathDissolveMaterials;

	FTimerHandle DeathDissolveTimer;
	double DeathDissolveStartTime = 0.0;
};
