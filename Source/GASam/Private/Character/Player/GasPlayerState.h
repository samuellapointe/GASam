// Samuel Lapointe - 2026

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySet.h"
#include "GameFramework/PlayerState.h"
#include "GasPlayerState.generated.h"

class UHealthAttributeSet;
class UManaAttributeSet;

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

	const UHealthAttributeSet* GetHealthSet() const { return HealthSet; };
	const UManaAttributeSet* GetManaSet() const { return ManaSet; };

	void GrantDefaultAbilities() const;
	void ApplyDefaultEffects() const;

	void RespawnCharacter() const;

protected:
	/* [INFO] In most cases you'll use a custom child class of UAbilitySystemComponent here to have more control over
	 * it, but for this simple example project it's not necessary. */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	/* [INFO] Abilities need to be granted before they can be used by a player. For this project and for simplicity,
	 * we just grant them all automatically from the start. In an actual game, you might grant them through player 
	 * upgrades, equipment or other gameplay moments. Note that a client cannot predict granting abilities, it must
	 * rely on the server to do it and wait for it to replicate the ability spec back.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrantOnStart;

	/* This is one of many ways we can reset the player's attributes on start. It has the advantage of being easy
	 * for designers to edit and simple to implement.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> EffectsToApplyOnStart;

private:
	/* Since the PlayerState owns the AbilitySystemComponent, it makes sense for it to also own its attribute sets.
	 * However, we could have chosen to place this in a dedicated Health Component instead.
	 */
	UPROPERTY()
	TObjectPtr<UHealthAttributeSet> HealthSet;

	UPROPERTY()
	TObjectPtr<UManaAttributeSet> ManaSet;
};
