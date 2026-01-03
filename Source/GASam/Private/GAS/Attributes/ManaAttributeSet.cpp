// Samuel Lapointe - 2026


#include "ManaAttributeSet.h"

#include "Net/UnrealNetwork.h"

UManaAttributeSet::UManaAttributeSet()
{
	InitMana(100.f);
	InitMaxMana(100.f);
}

void UManaAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UManaAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UManaAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UManaAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UManaAttributeSet, Mana, OldMana);
}

void UManaAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UManaAttributeSet, MaxMana, OldMaxMana);
}
