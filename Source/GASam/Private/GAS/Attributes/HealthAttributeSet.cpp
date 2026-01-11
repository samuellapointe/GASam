// Samuel Lapointe - 2026


#include "GAS/Attributes/HealthAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "GASamGameplayTags.h"
#include "Net/UnrealNetwork.h"

UHealthAttributeSet::UHealthAttributeSet()
{
	/* [INFO] There are many ways to initialize the default values for attributes. This is the simplest, but also
	 * the least accessible to designers. This is an example only, as our GasPlayerState contains an array of
	 * GameplayEffects to apply on start, which is what we actually use to initialize our attributes in this project.
	 * For more information: https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine-gameplay-ability-system-best-practices-for-setup#howshouldiinitializeattributevalues?
	 */
	InitHealth(20.f);
	InitMaxHealth(20.f);
}

void UHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, Health, OldHealth);
}

void UHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, MaxHealth, OldMaxHealth);
}

void UHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), 0.f, GetMaxHealth()));
	}

	if (GetHealth() <= 0.f && !bOutOfHealth)
	{
		bOutOfHealth = true;
		FireDeathEvent();
	}
	else if (bOutOfHealth && GetHealth() > 0.f) // Triggers on respawn, when our init gameplay effect refills health
	{
		bOutOfHealth = false;
	}
}

void UHealthAttributeSet::FireDeathEvent() const
{
	if (UAbilitySystemComponent* OwnerASC = GetOwningAbilitySystemComponent())
	{
		if (!OwnerASC->IsOwnerActorAuthoritative())
		{
			return;
		}
		
		FGameplayEventData EventData;
		EventData.EventTag = GASamGameplayTags::GASam_GameplayEvent_Death;
		EventData.Target = OwnerASC->GetAvatarActor();
		OwnerASC->HandleGameplayEvent(EventData.EventTag, &EventData);
	}
}