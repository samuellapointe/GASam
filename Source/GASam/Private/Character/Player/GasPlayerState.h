// Samuel Lapointe - 2025

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "GasPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class GASAM_API AGasPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGasPlayerState();
	
	// ~Begin IAbilitySystemInterface
	/* [INFO] Implementing IAbilitySystemInterface allows UAbilitySystemGlobals::GetAbilitySystemComponentFromActor
	 * to easily reach the ASC. This function is called by many of the blueprint-accessible ASC functions, which
	 * take an actor as parameter and use it to find the associated ASC. If not implemented, it will fallback on
	 * looking through the actor's components to find the ASC, which is less performant. You should implement this
	 * in your avatar actor and owner actor. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~End IAbilitySystemInterface

protected:
	/* [INFO] In most cases you'll use a custom child class of UAbilitySystemComponent here to have more control over
	 * it, but for this simple example project it's not necessary. */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
