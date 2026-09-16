// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraabilitySystemComponent.h"
#include "AbilitySystem/ModMagCalc/AuraAbilitySystemLibrary.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates= true;
	
	Spline=CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	if (!IsLocalController()) return;
	
	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning || !IsValid(Spline) || Spline->GetNumberOfSplinePoints() < 2)
	{
		bAutoRunning = false;
		return;
	}
	
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		bAutoRunning = false;
		return;
	}
	
	const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
		ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
	const FVector Direction = Spline->FindDirectionClosestToWorldLocation(
		LocationOnSpline, ESplineCoordinateSpace::World);
	ControlledPawn->AddMovementInput(Direction);
	
	if (FVector::Dist(LocationOnSpline, CatchDestination) <= AutoRunAcceptanceRadius)
	{
		bAutoRunning = false;
	}
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
	
	AActor* HitActor = CursorHit.GetActor();

	ThisActor =IsValid(HitActor) && HitActor->Implements<UEnemyInterface>()? HitActor: nullptr;

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
	
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bAutoRunning=false;
		bTargeting=ThisActor.IsValid();
	}
	
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}
	
	if (bTargeting)
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}else
	{
		APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			UWorld* World = GetWorld();
			UNavigationPath* NavPath = World
				? UNavigationSystemV1::FindPathToLocationSynchronously(
					World, ControlledPawn->GetActorLocation(), CatchDestination, ControlledPawn)
				: nullptr;
			
			if (IsValid(NavPath) && NavPath->IsValid() && NavPath->PathPoints.Num() > 1)
			{
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					DrawDebugSphere(World, PointLoc, 10.f, 8, FColor::Green, false, 5.f);
				}
				CatchDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
				bAutoRunning = true;
			}
			else
			{
				bAutoRunning = false;
				UE_LOG(LogTemp, Warning, TEXT("[AutoRun] No valid navigation path on %s"), *GetNameSafe(this));
			}
		}
		FollowTime=0.f;
		bTargeting=false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}
	
	if (bTargeting)
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}
	else
	{
		FollowTime+=GetWorld()->GetDeltaSeconds();
		
		FHitResult Hit;
		if (GetHitResultUnderCursor(ECC_Visibility,false,Hit))
		{
			CatchDestination=Hit.ImpactPoint;
		}
		
		if (APawn* ControlledPawn=GetPawn())
		{
			const FVector WorldDirection=(CatchDestination-ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

UAuraabilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AbilitySystemComponent==nullptr)
	{
		AbilitySystemComponent=Cast<UAuraabilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AbilitySystemComponent;
}
