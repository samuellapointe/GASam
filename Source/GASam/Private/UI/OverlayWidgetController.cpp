// Samuel Lapointe - 2026


#include "OverlayWidgetController.h"

#include "Character/Player/GasPlayerState.h"
#include "GAS/Attributes/HealthAttributeSet.h"
#include "GAS/Attributes/ManaAttributeSet.h"

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

	if (const UManaAttributeSet* ManaSet = GetManaAttributeSet())
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			ManaSet->GetManaAttribute()).AddUObject(this, &UOverlayWidgetController::ManaChanged);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			ManaSet->GetMaxManaAttribute()).AddUObject(this, &UOverlayWidgetController::MaxManaChanged);
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

	if (const UManaAttributeSet* ManaSet = GetManaAttributeSet())
	{
		OnManaChanged.Broadcast(ManaSet->GetMana());
		OnMaxManaChanged.Broadcast(ManaSet->GetMaxMana());
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

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}

const UHealthAttributeSet* UOverlayWidgetController::GetHealthAttributeSet() const
{
	check (PlayerState);

	return PlayerState->GetHealthSet();
}

const UManaAttributeSet* UOverlayWidgetController::GetManaAttributeSet() const
{
	check (PlayerState);

	return PlayerState->GetManaSet();
}
