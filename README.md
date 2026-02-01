# GASam

### NOTE: This is an incomplete work in progress!

This is an example project meant to showcase a minimal setup of Unreal's Gameplay Ability System project with as least 
clutter as possible, along with some examples of abilities, effects and cues. It was made using the "Blank" project 
template in Unreal 5.6.1.

### Testing the game
Controls:
* Movement: WASD keys, or arrow keys
* Jump: Spacebar
* Camera: Mouse movement
* V: Debug apply damage to self

### Browsing the code:
Throughout the project, useful information about GAS concepts has been marked in comments with `[INFO]`

#### Suggested browsing order:
1. Initialization of the ASC
   1. [GasPlayerState.h](Source/GASam/Private/Character/Player/GasPlayerState.h), which is where the player's Ability 
      System lives.
   2. [AGasPlayerState::AGasPlayerState()](Source/GASam/Private/Character/Player/GasPlayerState.cpp) for the 
      instantiation
   3. [AGasPlayerCharacter::InitializeAbilitySystemComponent](Source/GASam/Private/Character/Player/GasPlayerCharacter.cpp) for the initialization
2. Creation of the attributes
   1. [HealthAttributeSet.h](Source/GASam/Private/GAS/Attributes/HealthAttributeSet.h) and its cpp file for an 
      example of an attribute set. ManaAttributeSet is very similar although simpler. They get instantiated in the 
      PlayerState's constructor.


## Detailed Walkthrough
### 1. Setting up the project
Open your project's *.Build.cs file and add the following to your public dependency modules:
* GameplayAbilities
* GameplayTags
* GameplayTasks

Example:
```cs
PublicDependencyModuleNames.AddRange(new string[] 
{
   "Core", 
   "CoreUObject", 
   "Engine", 
   "EnhancedInput", 
   "InputCore", 
   "GameplayAbilities",
   "GameplayTags",
   "GameplayTasks"
});
```

### 2. Adding an Ability System Component
The Ability System Component, which we'll also refer to as the ASC, is a `UActorComponent` that handles most 
of the logic in the Gameplay Ability System. Our player will definitely have one, and so will any enemies that we 
want to have interact with abilities, effects and attributes, although that won't be the case in this example project.

The ASC has an `OwnerActor` which is the true owner, and an `AvatarActor` which is an in-game representation of the 
ASC's owner. These can be the same such as the character actor, but we'll often use the PlayerState as the 
`OwnerActor`. Doing so allows us to have attributes, abilities and effects persist even if the character actor 
respawns. 

#### 2.1. Setting up the PlayerState
Open your PlayerState's header file and add an entry for the AbilitySystemComponent: 
```h
UPROPERTY()
TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
```
and create a default subobject for it in the PlayerState's constructor:
```cpp
AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
AbilitySystemComponent->SetIsReplicated(true);
AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
```
For an explanation on the replication mode, see the following documentation: https://github.com/tranek/GASDocumentation?tab=readme-ov-file#411-replication-mode

You'll also need to increase the PlayerState's NetUpdateFrequency since its default value is quite low and it'll 
affect the responsiveness of the ASC, and by extension, the attributes, effects and abilities.
```cpp
SetNetUpdateFrequency(100.f);
```

You should also have your PlayerState implement the `IAbilitySystemInterface` interface and implement its 
`GetAbilitySystemComponent` function. A lot of GAS systems will expect the OwnerActor and AvatarActor to have this 
implemented.

```h
virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
```

#### 2.2. Setting up the Character
The ASC is created but still needs to be initialized. Both the server and clients will need to do this, so we want 
to hook up to the appropriate functions for each.

First, create a function to handle the initialization of the ASC in your character class. As an example:
```cpp
void AGasPlayerCharacter::InitializeAbilitySystemComponent()
{
	if (AGasPlayerState* GasPlayerState = GetPlayerStateChecked<AGasPlayerState>())
	{
	    // Here, AbilitySystemComponent is a non-owning TObjectPtr that we save for convenience
		AbilitySystemComponent = GasPlayerState->GetAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(GasPlayerState, this);
	}
}
```

InitAbilityActorInfo is the function setting both the `OwnerActor` (which never changes) and the `AvatarActor` on 
the ASC. Having a null AvatarActor is fully supported, for example while the player is respawning.

Then, override and implement `PossessedBy` and `OnRep_PlayerState` on your character. Both will call the 
initialization function introduced above. PossessedBy is only called by the server and OnRep_PlayerState is only 
called by the client.

```cpp
void AGasPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitializeAbilitySystemComponent();
}

void AGasPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitializeAbilitySystemComponent();
}
```

With this, the ASC is ready to use and it's up to you to decide what to do with it.