// Samuel Lapointe - 2026


#include "GAS/Abilities/DeathAbility.h"

#include "AbilitySystemComponent.h"
#include "Character/Player/GasPlayerState.h"

UDeathAbility::UDeathAbility()
{
	/* [INFO] Abilities will often be triggered by the client, in which case we would use the
	 * LocalPredicted NetExecutionPolicy. In this particular case, death is triggered when a player runs out of health,
	 * and we want health to be server-authoritative. For this reason, it's initiated by the server only.
	 */
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UDeathAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	/* [INFO] For this ability, we'll handle cleaning up the ASC and respawning the character in code, but we'll
	 * let a blueprint implementation decide what to do in terms of visuals until it calls EndAbility.
	 * We'll be triggering this ability using a Gameplay Event and using the ActivateAbilityFromEvent
	 * node in blueprint, which is triggered by UGameplayAbility::ActivateAbility.
	 */
	UAbilitySystemComponent* TargetASC = ActorInfo->AbilitySystemComponent.Get();
	check(TargetASC);

	// Note: you could filter by tag here, see UAbilitySystemComponent::CancelAbilities
	TargetASC->CancelAllAbilities(this);
	
	// This will call ActivateAbilityFromEvent which we implement in blueprint
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UDeathAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Authority)
	{
		if (AGasPlayerState* GasPlayerState = Cast<AGasPlayerState>(ActorInfo->OwnerActor))
		{
			GasPlayerState->RespawnCharacter();
		}
	}
}
