// Samuel Lapointe - 2026

#pragma once

#include "CoreMinimal.h"
#include "GasWidgetController.h"
#include "OverlayWidgetController.generated.h"

struct FOnAttributeChangeData;
class UHealthAttributeSet;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedDelegate, float, NewValue);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GASAM_API UOverlayWidgetController : public UGasWidgetController
{
	GENERATED_BODY()

public:
	virtual void InitializeController(APlayerState* InPlayerState) override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedDelegate OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedDelegate OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedDelegate OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedDelegate OnMaxManaChanged;

protected:
	void HealthChanged(const FOnAttributeChangeData& Data) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	
private:
	const UHealthAttributeSet* GetHealthAttributeSet() const;
};
