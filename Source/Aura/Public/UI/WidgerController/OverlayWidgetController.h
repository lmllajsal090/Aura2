// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgerController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

class UAuraUserWidget;
class UOverlayWidgetController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangeSignature,float,NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangeSignature,float,NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChangeSignature,float,NewMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxManaChangeSignature,float,NewMaxMana);

USTRUCT(BlueprintType)
struct FUIWidgetRow:public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag MessageTag=FGameplayTag();
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FText Message=FText();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

/**
 *
 */
UCLASS(BlueprintType,Blueprintable)

class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitiaValue() override;
	virtual void BindCallbacksToDependencies()override;
	
	UPROPERTY(BlueprintAssignable,Category="GAS|Attributes")
	FOnHealthChangeSignature OnHealthChange;
	
	UPROPERTY(BlueprintAssignable,Category="GAS|Attributes")
	FOnMaxHealthChangeSignature OnMaxHealthChange;
	
	UPROPERTY(BlueprintAssignable,Category="GAS|Attributes")
	FOnHealthChangeSignature OnManaChange;
	
	UPROPERTY(BlueprintAssignable,Category="GAS|Attributes")
	FOnMaxHealthChangeSignature OnMaxManaChange;
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
	
	
	void HealthChanged(const FOnAttributeChangeData& Data) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	void ManaChanged(const FOnAttributeChangeData& Data) const;
	void MaxManaChanged(const FOnAttributeChangeData& Data)const;
	
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	
	return DataTable->FindRow<T>(Tag.GetTagName(),TEXT(""));
	
}
