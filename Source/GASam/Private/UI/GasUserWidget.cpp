// Samuel Lapointe - 2026


#include "UI/GasUserWidget.h"

void UGasUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	OnWidgetControllerSet();
}
