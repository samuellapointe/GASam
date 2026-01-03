// Samuel Lapointe - 2026

#pragma once

#include "CoreMinimal.h"
#include "GasWidgetController.generated.h"

class AGasPlayerState;
class UAbilitySystemComponent;
class UAttributeSet;

/**
 * A simple UI controller, responsible for fetching data from the model (GAS attributes in this case), and broadcasting
 * it to the widgets controlled by this controller. The controller doesn't know about its widgets, the widgets know
 * about the controller.
 */
UCLASS()
class UGasWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void InitializeController(APlayerState* InPlayerState);
	virtual void BroadcastInitialValues();
	
protected:
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<AGasPlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
