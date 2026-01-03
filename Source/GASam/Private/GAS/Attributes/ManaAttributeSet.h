// Samuel Lapointe - 2026

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BaseAttributeSet.h"
#include "ManaAttributeSet.generated.h"

/**
 * An attribute set for "mana", which is a resource spent to activate some abilities.
 */
UCLASS()
class GASAM_API UManaAttributeSet : public UBaseAttributeSet
{
	GENERATED_BODY()

public:
	UManaAttributeSet();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	ATTRIBUTE_ACCESSORS(UManaAttributeSet, Mana);
	ATTRIBUTE_ACCESSORS(UManaAttributeSet, MaxMana);

protected:
	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);

	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);

private:
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana, Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Mana;

	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_MaxMana, Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData MaxMana;
};
