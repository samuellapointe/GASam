// Samuel Lapointe - 2026


#include "UI/GasWidgetController.h"

#include "Character/Player/GasPlayerState.h"

void UGasWidgetController::InitializeController(APlayerState* InPlayerState)
{
	check(InPlayerState);

	if (AGasPlayerState* InGasPlayerState = Cast<AGasPlayerState>(InPlayerState))
	{
		PlayerState = InGasPlayerState;
		PlayerController = InPlayerState->GetPlayerController();
		AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
	}
}

void UGasWidgetController::BroadcastInitialValues()
{
}
