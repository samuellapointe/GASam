// Samuel Lapointe - 2026


#include "LightningBoltAbility.h"

#include "GASamGameplayTags.h"
#include "GAS/GASDeveloperSettings.h"
#include "GAS/Effects/LightningBoltCostEffect.h"
#include "GAS/Effects/LightningDamageEffect.h"
#include "Objects/LightningBolt.h"

ULightningBoltAbility::ULightningBoltAbility()
{
	/* Only the server can spawn actors that get replicated. In a more involved implementation, we would likely spawn
	 * a fake projectile on the client and have it lerp to the server's version then disappear. For this simple project,
	 * we'll just accept visible latency for the client. */
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	// We'll be using a child Blueprint class of ALightningBolt for convenience
	const UGASDeveloperSettings* GasSettings = GetDefault<UGASDeveloperSettings>();
	ProjectileClass = GasSettings && GasSettings->LightningBoltClass ? GasSettings->LightningBoltClass.Get() : ALightningBolt::StaticClass();

	// Default to our Damage Effect class, this project won't extend ULightningDamageEffect but it could.
	if (!EffectClass)
	{
		EffectClass = ULightningDamageEffect::StaticClass();
	}

	// We'll be using a tag to trigger the ability
	FGameplayTagContainer TagsToSet = FGameplayTagContainer();
	TagsToSet.AddTag(GASamGameplayTags::GASam_Ability_LightningBolt);
	SetAssetTags(TagsToSet);

	// Mana cost, hardcoded in C++ for the sake of a code-only example
	CostGameplayEffectClass = ULightningBoltCostEffect::StaticClass();
}

void ULightningBoltAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	 // The Net Execution policy should ensure that we have authority.
	check(HasAuthority(&ActivationInfo));

	/* ActivateAbility is called after checking that the cost (if there is one) can be afforded. However we still need
	 * to call it to actually spend that cost. If we don't have the resources anymore, end the ability otherwise it
	 * will stay active. */
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		constexpr bool bReplicateEndAbility = true;
		constexpr bool bWasCancelled = false;
		EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}

	SpawnProjectile();

	constexpr bool bReplicateEndAbility = true;
	constexpr bool bWasCancelled = false;
	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULightningBoltAbility::SpawnProjectile()
{
	APawn* PawnInstigator = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!PawnInstigator)
	{
		return;
	}

	if (!ProjectileClass)
	{
		return;
	}

	FGameplayEffectSpecHandle DamageSpec = PrepareDamageEffect();
	if (!DamageSpec.IsValid())
	{
		return;
	}

	FTransform SpawnTransform = GetSpawnTransform(*PawnInstigator);
	AActor* Owner = nullptr;
	if (ALightningBolt* Projectile = Cast<ALightningBolt>(GetWorld()->SpawnActorDeferred<AActor>(ProjectileClass, SpawnTransform, Owner, PawnInstigator)))
	{
		Projectile->Initialize(DamageSpec);
		Projectile->FinishSpawning(SpawnTransform);
	}
}

FTransform ULightningBoltAbility::GetSpawnTransform(const APawn& PawnInstigator)
{
	FVector SpawnPosition;
	FRotator SpawnRotation;
	PawnInstigator.GetActorEyesViewPoint(SpawnPosition, SpawnRotation);
	SpawnRotation.Pitch = 0.f;
	
	return FTransform(SpawnRotation, SpawnPosition);
}

FGameplayEffectSpecHandle ULightningBoltAbility::PrepareDamageEffect() const
{
	if (EffectClass == nullptr)
	{
		return nullptr;
	}

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass, GetAbilityLevel());
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(GASamGameplayTags::GASam_Damage, AbilityDamage);
	return SpecHandle;
}
