// Samuel Lapointe - 2025


#include "GasPlayerState.h"

#include "AbilitySystemComponent.h"

AGasPlayerState::AGasPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	/* [INFO] See the following documentation for information about the replication mode:
	 * https://github.com/tranek/GASDocumentation?tab=readme-ov-file#411-replication-mode
	 */
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UAbilitySystemComponent* AGasPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
