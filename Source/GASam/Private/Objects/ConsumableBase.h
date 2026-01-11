// Samuel Lapointe - 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConsumableBase.generated.h"

class UGameplayEffect;

UCLASS(Blueprintable, BlueprintType)
class AConsumableBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AConsumableBase();

	UFUNCTION(BlueprintCallable)
	void TryConsume(AActor* Target);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> AppliedGameplayEffects;

};
