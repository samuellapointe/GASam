// Samuel Lapointe - 2026

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BaseAttributeSet.h"
#include "HealthAttributeSet.generated.h"

/**
 * [INFO] Attribute sets can be split into many themed sets or you can have one big attribute set containing every
 * attribute in the game. For organization and readability purpose, this project splits them by attribute, where
 * one set contains the relevant attribute, the attribute for its max value and any relevant meta attributes.
 */
UCLASS()
class UHealthAttributeSet : public UBaseAttributeSet
{
	GENERATED_BODY()
	
public:
	UHealthAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Damage);

protected:
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	
	void FireDeathEvent() const;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health, Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth, Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData MaxHealth;

	/* [INFO] This is a meta attribute. It resets every frame and its purpose is to allow additional processing
	 * where it makes sense. For example, a raw value of incoming damage could be modified by a shield or a damage
	 * type resistance before being applied as negative health. These are generally not replicated.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Lyra|Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Damage;

	// Used to track when the health reaches 0, preventing the death event from firing twice.
	bool bOutOfHealth = false;
	
};
