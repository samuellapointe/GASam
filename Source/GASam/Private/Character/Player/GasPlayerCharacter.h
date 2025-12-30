// Samuel Lapointe - 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GasPlayerCharacter.generated.h"

class UInputAction;
class UInputComponent;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class AGasPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess=true))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess=true))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess=true))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess=true))
	TObjectPtr<UInputAction> JumpAction;
};
