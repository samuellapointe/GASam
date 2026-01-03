// Samuel Lapointe - 2026


#include "OverlayWidgetController.h"

#include "Character/Player/GasPlayerState.h"
#include "GAS/Attributes/HealthAttributeSet.h"

void UOverlayWidgetController::InitializeController(APlayerState* InPlayerState)
{
	Super::InitializeController(InPlayerState);

	if (const UHealthAttributeSet* HealthSet = GetHealthAttributeSet())
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			HealthSet->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			HealthSet->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);
	}
}

void UOverlayWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();

	if (const UHealthAttributeSet* HealthSet = GetHealthAttributeSet())
	{
		OnHealthChanged.Broadcast(HealthSet->GetHealth());
		OnMaxHealthChanged.Broadcast(HealthSet->GetMaxHealth());
	}
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

const UHealthAttributeSet* UOverlayWidgetController::GetHealthAttributeSet() const
{
	check (PlayerState);

	return PlayerState->GetHealthSet();
}
