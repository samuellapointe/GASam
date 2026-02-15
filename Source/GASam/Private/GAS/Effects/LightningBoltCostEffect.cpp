// Samuel Lapointe - 2026


#include "LightningBoltCostEffect.h"

#include "GAS/Attributes/ManaAttributeSet.h"

ULightningBoltCostEffect::ULightningBoltCostEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = UManaAttributeSet::GetManaAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;
	Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(-15.f);

	Modifiers.Add(Modifier);
}
