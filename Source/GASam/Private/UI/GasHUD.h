// Samuel Lapointe - 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GasHUD.generated.h"

class UGasUserWidget;
class UOverlayWidgetController;

/**
 * 
 */
UCLASS()
class AGasHUD : public AHUD
{
	GENERATED_BODY()
	
public:
    UPROPERTY()
	TObjectPtr<UGasUserWidget> OverlayWidget;

	UOverlayWidgetController* GetOverlayWidgetController(APlayerState* Owner);

	void InitOverlay(APlayerState* Owner);

private:
    UPROPERTY(EditAnywhere)
    TSubclassOf<UGasUserWidget> OverlayWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
};
