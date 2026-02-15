// Samuel Lapointe - 2026

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "LightningBoltAbility.generated.h"

class ALightningBolt;

/**
 * 
 */
UCLASS()
class ULightningBoltAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	ULightningBoltAbility();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ALightningBolt> ProjectileClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> EffectClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float AbilityDamage = 30.f;
	
private:
	void SpawnProjectile();
	static FTransform GetSpawnTransform(const APawn& PawnInstigator);
	FGameplayEffectSpecHandle PrepareDamageEffect() const;
};
