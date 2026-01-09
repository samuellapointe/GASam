// Samuel Lapointe - 2026


#include "GasPlayerState.h"

#include "AbilitySystemComponent.h"
#include "EditorCategoryUtils.h"
#include "GameFramework/GameModeBase.h"
#include "GAS/Attributes/HealthAttributeSet.h"
#include "GAS/Attributes/ManaAttributeSet.h"

AGasPlayerState::AGasPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	/* See the following documentation for information about the replication mode:
	 * https://github.com/tranek/GASDocumentation?tab=readme-ov-file#411-replication-mode
	 */
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	HealthSet = CreateDefaultSubobject<UHealthAttributeSet>("Health Attribute Set");
	ManaSet = CreateDefaultSubobject<UManaAttributeSet>("Mana Attribute Set");

	/* How often the server will try to update clients. The default value for the PlayerState is quite low,
	 * and since it owns the AbilitySystemComponent, we'll want it to have a higher net update frequency.
	 */
	SetNetUpdateFrequency(100.f);
}

UAbilitySystemComponent* AGasPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGasPlayerState::GrantDefaultAbilities() const
{
	check(AbilitySystemComponent);
	
	AbilitySystemComponent->ClearAllAbilities();
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : AbilitiesToGrantOnStart)
	{
		if (IsValid(AbilityClass))
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass));
		}
	}
}

void AGasPlayerState::ApplyDefaultEffects() const
{
	check(AbilitySystemComponent);
	for (const TSubclassOf<UGameplayEffect>& EffectClass : EffectsToApplyOnStart)
	{
		if (IsValid(EffectClass))
		{
			FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
			Context.AddSourceObject(this);

			constexpr int Level = 1.f;
			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, Level, Context);

			if (SpecHandle.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

void AGasPlayerState::RespawnCharacter() const
{
	if (UWorld* World = GetWorld())
	{
		if (AGameModeBase* GameMode = World->GetAuthGameMode())
		{
			if (APawn* Pawn = GetPawn())
			{
				Pawn->Destroy();
			}
			
			GameMode->RestartPlayer(GetOwningController());
		}
	}
}
