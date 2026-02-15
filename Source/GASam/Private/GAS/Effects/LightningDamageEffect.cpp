// Samuel Lapointe - 2026


#include "GAS/Effects/LightningDamageEffect.h"

#include "GASamGameplayTags.h"
#include "GAS/Attributes/HealthAttributeSet.h"

ULightningDamageEffect::ULightningDamageEffect()
{
	/* [INFO] Setting up a gameplay effect in code like this is unwieldy and unintuitive to the designers.
	 * It is provided as an example but not recommended as it bypasses the usability benefits of the GAS workflow.
	 */
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = UHealthAttributeSet::GetDamageAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;

	// Damage is transferred using a SetByCaller magnitude
	FSetByCallerFloat SetByCallerDamage = FSetByCallerFloat();
	SetByCallerDamage.DataTag = GASamGameplayTags::GASam_Damage;
	Modifier.ModifierMagnitude = FSetByCallerFloat(SetByCallerDamage);

	Modifiers.Add(Modifier);
}
