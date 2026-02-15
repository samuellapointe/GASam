// Samuel Lapointe - 2026

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GASDeveloperSettings.generated.h"

class ALightningBolt;

/**
 * 
 */
UCLASS(Config=Game, defaultconfig, meta = (DisplayName="Gameplay Ability System Developer Settings"))
class UGASDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	/* The Lightning Bolt ability is an example of a code only ability and effect setup, but dealing with visuals
	 * and materials in code would be a bit too much. We'll inherit from ALightningBolt with a BlueprintClass to
	 * setup the visuals, and spawn that class in our LightningBolt ability.
	 * 
	 * With a mix of blueprints and code, we could have placed this property in a code base class of the LightningBolt
	 * ability and overriden it with a Blueprint version instead of using Developer Settings.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<ALightningBolt> LightningBoltClass;
};
