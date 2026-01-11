// Samuel Lapointe - 2026

#include "Objects/ConsumableBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AConsumableBase::AConsumableBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
}

void AConsumableBase::TryConsume(AActor* Target)
{
	bool bWasConsumed = false;
	
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
	{
		FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		
		for (TSubclassOf<UGameplayEffect>& EffectClass : AppliedGameplayEffects)
		{
			if (!IsValid(EffectClass))
			{
				continue;
			}

			constexpr float Level = 1.0f;
			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(EffectClass, Level, EffectContext);
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}

		bWasConsumed = true;
	}

	if (bWasConsumed)
	{
		Destroy();
	}
}

void AConsumableBase::BeginPlay()
{
	Super::BeginPlay();
}
