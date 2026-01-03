// Samuel Lapointe - 2026

#include "UI/GasHUD.h"

#include "OverlayWidgetController.h"
#include "UI/GasUserWidget.h"

UOverlayWidgetController* AGasHUD::GetOverlayWidgetController(APlayerState* OwningPlayerState)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->InitializeController(OwningPlayerState);
	}

	return OverlayWidgetController;
}

void AGasHUD::InitOverlay(APlayerState* OwningPlayerState)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget class not set"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller class not set"));

	UOverlayWidgetController* Controller = GetOverlayWidgetController(OwningPlayerState);
	check(Controller);
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UGasUserWidget>(Widget);
	OverlayWidget->SetWidgetController(Controller);
	Controller->BroadcastInitialValues();
	OverlayWidget->AddToViewport();
}

