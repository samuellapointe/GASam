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

With this, the ASC is ready to use, and it's up to you to decide what to do with it.

### 3. Using attributes

If you want your character to have health, mana, or any numerical characteristic and if you want those 
characteristics to interact and be affected by the GameplayAbilitySystem, you need attributes. 

In simplest terms, an attribute is a float value bound to an actor. It is stored in a UAttributeSet and owned by an 
AbilitySystemComponent. It has a BaseValue and a CurrentValue, the latter being the result of the BaseValue plus 
temporary modifications from active GameplayEffects and other GAS systems. For a more detailed explanation, see the 
following documentation: https://github.com/tranek/GASDocumentation?tab=readme-ov-file#concepts-a

#### 3.1. Setting up an AttributeSet

An AttributeSet is a `UObject` containing definitions for Attributes. It also handles the replication of these 
attributes as a SubObject of the AbilitySystemComponent's OwnerActor, although you still need to implement the proper 
replication calls.

First, you'll want to add the following macros (from AttributeSet.h) to your AttributeSet's header file:
```h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
   GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
   GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
   GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
   GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
```

Alternatively, you can create a Base AttributeSet class for your project and include this macro there. It's not 
mandatory to use these macros, but it will save a lot of time and this walkthrough will assume you included them.

#### 3.2. Defining a single attribute

For a single attribute named Health, we would define it in the header file like so:
```h
public:
   ATTRIBUTE_ACCESSORS(UMyAttributeSet, Health);
   
protected:
   virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

   UFUNCTION()
   void OnRep_Health(const FGameplayAttributeData& OldHealth);
	
private:
   UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health, Meta=(AllowPrivateAccess=true))
   FGameplayAttributeData Health;
```

and in the cpp file:
```cpp
void UMyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);

   DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Health, COND_None, REPNOTIFY_Always);
}

void UMyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Health, OldHealth);
}
```

However, attributes like Health generally come in pairs such as Health and MaxHealth. This is because while a single 
attribute has a BaseValue and a CurrentValue, those refer to the value before and after the gameplay effects are 
applied. Whenever we fetch the value of an attribute, we're usually looking for the CurrentValue.

An attribute such as MaxHealth will allow you to implement having the player "refill" their health up to a defined 
maximum (which can potentially be increased in game), which is what most games do with "resource" attributes. In 
contrast, attributes such as a movement speed don't refill or deplete, so they don't need to track a maximum.

#### 3.3. Adding attributes to the character

With an attribute defined in an attribute set, we can add it to our character. Since our AbilitySystemComponent is 
owned by our PlayerState, we'll place our AttributeSets in the PlayerState as well. Note that we add whole 
AttributeSets to an ASC-owning actor, not individual attributes within that set.

In the header file:
```
private:
	UPROPERTY()
	TObjectPtr<UHealthAttributeSet> HealthSet;
```

In the constructor:
```cpp
AGasPlayerState::AGasPlayerState()
{
   [...]
   HealthSet = CreateDefaultSubobject<UHealthAttributeSet>("Health Attribute Set");
}
```

Creating the AttributeSet as a Default SubObject ensures the AttributeSet exists before initial replication, allowing 
clients to bind onto their delegates immediately. Creating them in the OwnerActor's constructor also lets the ASC 
find them automatically during `UAbilitySystemComponent::InitializeComponent()`, when it looks through all its 
OwnerActor's default SubObjects for AttributeSets. Otherwise, we would need to call ASC->AddSet<T> manually.

At this point, the attribute will be functional and present on your player character. You may want to implement initialization, clamping functions or delegates on these attributes, in which case I would recommend looking at this documentation from Epic: https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine-gameplay-ability-system-best-practices-for-setup#2attributesandattributesets

