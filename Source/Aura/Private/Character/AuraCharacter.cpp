// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "TimerManager.h"

namespace
{
	constexpr float DeathDissolveDelay = 0.75f;
	constexpr float DeathDissolveDuration = 1.8f;
	constexpr float DeathDissolveTickInterval = 1.f / 60.f;
	const FName DeathDissolveParameter(TEXT("Dissolve"));

	void StartDeathPhysics(USkeletalMeshComponent& Component)
	{
		Component.SetGenerateOverlapEvents(false);
		Component.SetCollisionResponseToAllChannels(ECR_Ignore);
		Component.SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		Component.SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Component.SetEnableGravity(true);
		Component.SetSimulatePhysics(true);
	}

	void CreateDeathDissolveMaterials(USkeletalMeshComponent& Component,
		const TArray<TObjectPtr<UMaterialInterface>>& Templates,
		TArray<TObjectPtr<UMaterialInstanceDynamic>>& Materials)
	{
		for (int32 Index = 0; Index < Component.GetNumMaterials(); ++Index)
		{
			if (Templates.IsValidIndex(Index) && Templates[Index])
			{
				if (UMaterialInstanceDynamic* Material = Component.CreateDynamicMaterialInstance(Index, Templates[Index]))
				{
					Materials.Add(Material);
				}
			}
		}
	}

	void FinishDeathPresentation(USkeletalMeshComponent& Component)
	{
		Component.SetVisibility(false, true);
		Component.SetSimulatePhysics(false);
		Component.SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}


AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	bUseControllerRotationPitch=false;
	bUseControllerRotationRoll=false;
	bUseControllerRotationYaw=false;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

int32 AAuraCharacter::GetPlayerLevel()
{
	AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}


void AAuraCharacter::MulticastHandleDeath_Implementation()
{
	if (bDead)
	{
		return;
	}

	if (!DeathPhysicsAsset)
	{
		Super::MulticastHandleDeath_Implementation();
		return;
	}

	bDead = true;
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Weapon->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	StartDeathPhysics(*Weapon);

	USkeletalMeshComponent& CharacterMesh = *GetMesh();
	CharacterMesh.bDisableClothSimulation = true;
	CharacterMesh.SetPhysicsAsset(DeathPhysicsAsset);
	StartDeathPhysics(CharacterMesh);

	CreateDeathDissolveMaterials(CharacterMesh, BodyDeathMaterials, DeathDissolveMaterials);
	CreateDeathDissolveMaterials(*Weapon, WeaponDeathMaterials, DeathDissolveMaterials);
	DeathDissolveStartTime = GetWorld()->GetTimeSeconds() + DeathDissolveDelay;
	GetWorldTimerManager().SetTimer(DeathDissolveTimer, this, &AAuraCharacter::UpdateDeathDissolve,
		DeathDissolveTickInterval, true, DeathDissolveDelay);
}

void AAuraCharacter::UpdateDeathDissolve()
{
	const double Elapsed = GetWorld()->GetTimeSeconds() - DeathDissolveStartTime;
	const float Amount = FMath::Clamp(static_cast<float>(Elapsed) / DeathDissolveDuration, 0.f, 1.f);
	for (UMaterialInstanceDynamic* Material : DeathDissolveMaterials)
	{
		Material->SetScalarParameterValue(DeathDissolveParameter, Amount);
	}

	if (Amount >= 1.f)
	{
		GetWorldTimerManager().ClearTimer(DeathDissolveTimer);
		FinishDeathPresentation(*GetMesh());
		FinishDeathPresentation(*Weapon);
		DeathDissolveMaterials.Reset();
	}
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AbilitySystemComponent=AuraPlayerState->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState,this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	AttributeSet=AuraPlayerState->GetAttributeSet();
	
	//进行这个if判断是因为只有在本地客户端才能get到Controller
	if (AAuraPlayerController* AuraPlayerController=Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD=Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	
	InitializeDefaultAttributes();
}
 