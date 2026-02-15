// Samuel Lapointe - 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "LightningBolt.generated.h"

class UCapsuleComponent;
class UProjectileMovementComponent;

UCLASS()
class ALightningBolt : public AActor
{
	GENERATED_BODY()
	
public:	
	ALightningBolt();

	void Initialize(FGameplayEffectSpecHandle SpecHandle);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void ColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ApplyDamage(const AActor* HitActor) const;

	UPROPERTY(EditDefaultsOnly)
	float Speed = 4000.f;

	UPROPERTY()
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UShapeComponent> Collider;

private:

	FGameplayEffectSpecHandle DamageSpecHandle;
};
