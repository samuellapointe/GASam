// Samuel Lapointe - 2026


#include "Objects/LightningBolt.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ALightningBolt::ALightningBolt()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->ProjectileGravityScale = 0.1f;
}

void ALightningBolt::Initialize(FGameplayEffectSpecHandle SpecHandle)
{
	DamageSpecHandle = SpecHandle;
}

void ALightningBolt::BeginPlay()
{
	Super::BeginPlay();

	// Auto-Destroy if it takes too long to hit something
	SetLifeSpan(3.f);

	Collider = GetComponentByClass<UShapeComponent>();
	if (Collider)
	{
		Collider->OnComponentBeginOverlap.AddDynamic(this, &ALightningBolt::ColliderOverlap);
	}
}

void ALightningBolt::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (Collider)
	{
		Collider->OnComponentBeginOverlap.RemoveDynamic(this, &ALightningBolt::ColliderOverlap);
	}
}

void ALightningBolt::ColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
                                     int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Other == GetInstigator())
	{
		return;
	}

	ApplyDamage(Other);

	if (HasAuthority())
	{
		Destroy();
	}
}

void ALightningBolt::ApplyDamage(const AActor* HitActor) const
{
	if (!DamageSpecHandle.IsValid())
	{
		return;
	}
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);
	if (!TargetASC)
	{
		return;
	}

	TargetASC->ApplyGameplayEffectSpecToSelf(*DamageSpecHandle.Data.Get());
}


