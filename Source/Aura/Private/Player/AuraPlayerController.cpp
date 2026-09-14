// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AuraabilitySystemComponent.h"
#include "AbilitySystem/ModMagCalc/AuraAbilitySystemLibrary.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates= true;
	
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	checkf(AuraContext, TEXT("AuraContext 在编辑器里没有赋值！请检查 AuraPlayerController 蓝图！"));
	
	//单例模式，只存在一个增强玩家输入的本地指针系统
	UEnhancedInputLocalPlayerSubsystem* Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	if (Subsystem)
	{
		Subsystem->AddMappingContext(
			AuraContext,0);
	}
	
	bShowMouseCursor=true;
	DefaultMouseCursor=EMouseCursor::Default;
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	checkf(MoveAction, TEXT("AuraContext 在编辑器里没有赋值！请检查 AuraPlayerController 蓝图！"));
	
	UAuraInputComponent* AuraInputComponent=CastChecked<UAuraInputComponent>(InputComponent);
	
	AuraInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
	
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector=InputActionValue.Get<FVector2D>();
	const FRotator Rotation=GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw,0.f);
	
	const FVector ForwardDirection=FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection=FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);


	if (APawn* ControlledPawn=GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);
	
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit)
	{
		if (LastActor != nullptr)
		{
			if (IEnemyInterface* Enemy = Cast<IEnemyInterface>(LastActor.Get()))
			{
				Enemy->UnHighlightActor();
			}
			LastActor = nullptr;
			ThisActor = nullptr;
		}
		return;
	}

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			// Case B
			if (IEnemyInterface* Enemy = Cast<IEnemyInterface>(ThisActor.Get()))
			{
				Enemy->HighlightActor();
			}
		}
		// Case A 两者都空，无操作
	}
	else
	{
		if (ThisActor == nullptr)
		{
			// Case C
			if (IEnemyInterface* Enemy = Cast<IEnemyInterface>(LastActor.Get()))
			{
				Enemy->UnHighlightActor();
			}
		}
		else
		{
			if (LastActor != ThisActor)
			{
				// Case D
				if (IEnemyInterface* Enemy = Cast<IEnemyInterface>(LastActor.Get()))
				{
					Enemy->UnHighlightActor();
				}
				if (IEnemyInterface* Enemy = Cast<IEnemyInterface>(ThisActor.Get()))
				{
					Enemy->HighlightActor();
				}
			}
			// Case E 同一物体，无操作
		}
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC()==nullptr)return;
	GetASC()->AbilityInputTagReleased(InputTag);
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC()==nullptr)return;
	GetASC()->AbilityInputTagHeld(InputTag);
}

UAuraabilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AbilitySystemComponent==nullptr)
	{
		AbilitySystemComponent=Cast<UAuraabilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AbilitySystemComponent;
}

