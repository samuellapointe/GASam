# GASam

This is an example project meant to showcase a minimal setup of Unreal's Gameplay Ability System project with as least 
clutter as possible, along with some examples of abilities, effects and cues. It was made using the "Blank" project 
template in Unreal 5.6.1.

Down below is a written walkthrough on how to setup the Gameplay Ability System on a new project and how to set up a health attribute, a gameplay effect to initialize health to a default value and a Fireball gameplay ability that applies damage to other players. The end result of the guide is less intricate than the linked project, which also includes simple UI and FXs, but functions mostly the same way.

### Testing the game
Controls:
* Movement: WASD keys, or arrow keys
* Jump: Spacebar
* Camera: Mouse movement
* Q: Fireball ability (Blueprint ability example)
* E: Lightning bolt ability (C++ ability example)

### Browsing the code:
Throughout the project, useful information about GAS concepts has been marked in comments with `[INFO]`

## Detailed Walkthrough - Getting started with GAS
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

You'll also need to increase the PlayerState's NetUpdateFrequency since its default value is quite low (1hz) and it'll 
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

#### 3.4. Testing attributes

Having attributes is great, but until you implement a health bar, you need a way to check their values. There are two easy ways to do this in editor:

* Open the console (\`) then type ShowDebug AbilitySystem. Text should appear on the top right of the viewport with a list of attributes and their values. Note that you can then type `AbilitySystem.Debug.NextCategory` to cycle to a list of Gameplay Effects, then to a list of Gameplay Abilities.
* Press the Apostrophe (') key on US keyboards, or bind it to a key of your choice in the project settings (Engine -> Gameplay Debugger -> Activation Key). Some debug text will show up in the top left of the view port, press Numpad 3 to open a list of information related to GAS. There should be a list of your character's applied GameplayTags, active GameplayEffects and granted or active GameplayAbilities.

Read more on this here: https://github.com/tranek/GASDocumentation?tab=readme-ov-file#6-debugging-gas

### 4. Gameplay Effects

Gameplay Effects are the main mechanism you'll use to interact with your attributes, for example to heal your 
character with a health potion or to have your enemies experience status effects. Gameplay Effects are also able to 
add or remove GameplayTags, grant or block Gameplay Abilities, trigger additional Gameplay Effects and more. You'll 
find a detailed documentation on everything they can do here: https://github.com/tranek/GASDocumentation?tab=readme-ov-file#concepts-ge

For this walkthrough, we'll look at a simple gameplay effect to refill the player's health when the game starts.

#### 4.1. Code vs Blueprints

Gameplay Effects can be implemented in blueprints and in code, but I recommend sticking to blueprints unless you 
have a specific reason to use code, for example in complex calculations (see 4.2.4. Executions). Customizing your 
gameplay effects is much easier through the editor as there is UI and dropdowns allowing you to easily access all of a Gameplay Effect's features. 

For an example implementation of a Gameplay Effect in code, see [LightningDamageEffect.cpp]
(Source/GASam/Private/GAS/Effects/LightningDamageEffect.cpp). The rest of this documentation will use the editor route.

#### 4.2. Gameplay Effect creation and overview

In the Content Browser, right click to create a new asset and select Blueprint Class. Then, search for Gameplay 
Effect in the list of classes. When you open it, you'll see an empty event graph which we won't be using. Select the 
Class Defaults and look on the details pane, this is where you'll define the behavior of the gameplay effect.

##### 4.2.1. Duration Policy and Periodic Effects

The first setting you can define is the **Duration Policy**. 
* Instant is instantaneous, and can be used for one-shot modifications such as changing the target's attributes.
* Infinite is the opposite, it only ends when manually cancelled. Modifiers and granted tags are reverted when the 
  effect is removed.
* Has Duration is similar to Infinite but will remove itself after the given duration if it wasn't removed manually 
  by another system.

Selecting Infinite or Has Duration will also display a "Period" field, which is a time interval at which the effect's 
modifiers and executions are re-applied. By default, this is set to 0 which means no periodic effect will occur. 
Setting it to 0.5 and having it reduce the target's health would be a form of damage over time gameplay effect, as if an instant gameplay effect was applied every 0.5 second.

##### 4.2.2. Components

The next setting is a list of components, of which there can be 0 or as many as you want. These offer a variety of 
actions such as granting gameplay tags to the effect's targets or applying additional effects.

##### 4.2.3. Modifiers

The modifiers are the mechanism with which a gameplay effect will modify its target's attributes. You can select any 
attribute to be modified through various mathematical operations, for example adding -10 to a character's health 
attribute for a damage effect.

These are described in much more detail here: https://github.com/tranek/GASDocumentation?tab=readme-ov-file#concepts-ge-mods

##### 4.2.4. Executions

Executions are custom behavior implemented in C++ through the UGameplayEffectExecutionCalculation class. We'll see them 
used for more complex attribute modifications, such as calculating the impact damage on health based on a physics collision. They won't be used in this example project.

#### 4.3. Setting up an attribute initializer effect

A GameplayEffect can be a good designer-facing way to setup a player's attributes. To do this, create a new Gameplay 
Effect as described above, and set its duration to Instant. Add a modifier for each attribute you want to set, for 
example one for your Health attribute and one for MaxHealth. Set these modifiers to Override then define the desired 
value in the magnitude field, selecting the Scalable Float Calculation Type. When the effect takes place, the 
player's health attribute will instantly be overriden to the value you set here.

The next step is to have this gameplay effect execute when the game starts. One way to do this is to define an array 
of Gameplay Effect subclasses in your AbilitySystemComponent's owning class, the PlayerState in this project:

```h
protected:
    UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> EffectsToApplyOnStart;
```

Then, define a function in the player state to apply those effects, for example:

```cpp
void AGasPlayerState::ApplyDefaultEffects() const
{
	check(AbilitySystemComponent);
	for (const TSubclassOf<UGameplayEffect>& EffectClass : EffectsToApplyOnStart)
	{
		if (IsValid(EffectClass))
		{
			FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
			Context.AddSourceObject(this);

			constexpr int Level = 1.f;
			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, Level, Context);

			if (SpecHandle.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}
```

Then, call this function when the Ability System Component is ready, such as in the InitializeAbilitySystemComponent 
function created in section 2.2:

```cpp
void AGasPlayerCharacter::InitializeAbilitySystemComponent()
{
	if (AGasPlayerState* GasPlayerState = GetPlayerStateChecked<AGasPlayerState>())
	{
		AbilitySystemComponent = GasPlayerState->GetAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(GasPlayerState, this);

		if (HasAuthority())
		{
			GasPlayerState->ApplyDefaultEffects();
		}
	}
}
```

With this, you have your first gameplay effect and an editor-facing way to change your player's starting attributes.

#### 4.4. Applying Gameplay Effects in blueprints

We'll often use Gameplay Abilities (next section) to apply gameplay effects, but we can also apply them wherever we 
have access to an actor's AbilitySystemComponent. For example, in your character's blueprint, you can use the "Get 
Ability System Component" node to get the ASC from your character, then use the "ApplyGameplayEffectToSelf" to 
select a gameplay effect class to apply. The node will return a Gameplay Effect Handle, which you'll need to hold on 
to if you want to cancel that effect later if it's not instantaneous.

### 5. Gameplay Abilities

Gameplay Abilities are the implementation of actions that will be taken by Actors in a game using the Gameplay Ability System. These actions can be as simple as sprinting to move faster temporarily or jumping, it can be an ability that runs in the background and reacts when an attribute crosses a threshold; but for this particular walkthrough, we'll be implementing a fireball ability. It would be easy to implement such a feature without GAS in Unreal Engine, but using a Gameplay Ability will let us interact with our Attributes, apply Gameplay Effects, and it has additional benefits such as handling replication for us and having built-in support for cost and cooldown.

#### 5.1. Creating an ability in Blueprint

In my (relatively short) experience with GAS, it's easier to create an ability in the editor using Blueprints and have that ability call code functions if we need more complex behavior. Right click in the content browser, select Blueprint Class then look for Gameplay Ability. Open it, and you'll see the Event Graph with two event nodes ready to use; ActivateAbility and OnEndAbility.

A detailed flowchart of Gameplay Abilities can be found here: https://github.com/tranek/GASDocumentation?tab=readme-ov-file#concepts-ga-definition

But in short, ActivateAbility is where you'll want to start plugging your code, handling whatever logic you need to handle (such as spawning the projectile), and finishing by calling EndAbility. There's multiple ways to end an ability, a lot of which depend on which method you use to trigger that ability, but you want to call EndAbility when you know your logic has run and there's nothing more to do. Later, you'll also add a call to Commit Ability to support ability cost and cooldown.

#### 5.2. Creating a simple projectile class

To create a fireball ability, we'll need a fireball actor. Create a new blueprint class deriving for actor. Add a mesh of your choice such as a simple sphere and disable collisions on it. Add a Collider component that fits with the mesh, have it Generate Overlap Events, set its collision enabled (Query only) and have it overlap with Pawn (and other categories of your choice depending on the behavior you want). Then, give it a Projectile Movement component and set its Initial Speed to something like 1000. We'll leave it like this for now and come back to it later. Finally, mark your actor as Replicated so that when it spawns on the server, clients will receive a copy as well.

#### 5.3. Spawning the projectile

When handling projectiles in multiplayer games, the projectile will preferably be server-authoritative, and you'll want the client to see something right away until the actual projectile gets replicated back to them. This walkthrough won't go through such an implementation, but if you're interested, there is a good guide here: https://www.stevestreeting.com/2024/12/12/unreal-network-prediction-for-projectiles/
Alternatively, you can look at how Unreal Tournament did it in 1999, the idea on how to handle it didn't change much: https://github.com/JimmieKJ/unrealTournament/blob/clean-master/UnrealTournament/Source/UnrealTournament/Private/UTProjectile.cpp

For the sake of simplicity, we'll accept that the projectile is server authoritative and that the client will not see it right away. In your fireball ability's class defaults, set the Net Execution Policy to Server Only. Clients will still be able to trigger this ability, but it will only run on the server.

Create an empty function named SpawnProjectile. Connect it as follows: Event ActivateAbility -> SpawnProjectile -> EndAbility. (Later you'll add CommitAbility with an early EndAbility if it fails, see Cost and Cooldown). In the SpawnProjectile function, use the SpawnActor blueprint node to create an instance of your projectile blueprint at a location of your choosing, such as the instigating character's eyes view point which you can get with the node "Get Avatar Actor from Actor Info". With this done, we now have an ability that should spawn a server-authoritative projectile when activated.

#### 5.4. Granting abilities

Before we can consider activating abilities, we must grant it to the player. This happens on the Server and is replicated back to the client. The simplest way to start is to store an array of Abilities to grant on start, and grant them in our ASC initialization function.

In your PlayerState, define a variable for an array of Subclasses of UGameplayAbility, for example:

```cpp
UPROPERTY(EditDefaultsOnly, Category = "GAS")
TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrantOnStart;
```

as well as a boolean to track whether the abilities were granted, as we only want to grant them once:
```cpp
bool bStartupAbilitiesGranted = false;
```

Then create a function that will loop through this array and grant them using AbilitySystemComponent::GiveAbility:

```cpp
void AGasPlayerState::GrantDefaultAbilities()
{
	check(AbilitySystemComponent);
	
	if (bStartupAbilitiesGranted)
	{
		return;
	}
	
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : AbilitiesToGrantOnStart)
	{
		if (IsValid(AbilityClass))
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass));
		}
	}

	bStartupAbilitiesGranted = true;
}
```

You'll notice we're creating a FGameplayAbilitySpec and sending that instead of using the ability class directly. A Spec will allow you to pass additional parameters related to that ability such as its Level and its Source. We won't need to do this in this walkthrough.

Finally, add a call to your latest function in the ASC initialization function, right when we're applying the default Gameplay Effects:

```cpp
void AGasPlayerCharacter::InitializeAbilitySystemComponent()
{
    [...]
    
    if (HasAuthority())
    {
        GasPlayerState->GrantDefaultAbilities();
        GasPlayerState->ApplyDefaultEffects();
    }
}
```

The player will then be allowed to use abilities defined in this array. Don't forget to modify your blueprint PlayerState to add the Fireball ability class to the abilities to grant on start.

#### 5.5. Activating abilities

There are at least 6 different ways to activate an ability with GAS listed [here](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine-gameplay-ability-system-best-practices-for-setup#howdoilettheplayeractivateabilities?). This page is quite useful as it is, so I won't repeat its contents here. In this particular walkthrough, we'll be using the third method listed, activating via tag.

#### 5.5.1. Setting up input

For the specifics on how to have code executed on input, you can take a look inside AGasPlayerCharacter::SetupPlayerInputComponent in this project, but in summary:

In your character class:
* Expose a `TObjectPtr<UInputAction>` that you'll use for activating the ability
* Bind it to a new function of your choice, such as "UsePrimaryAbility", using `EnhancedInputComponent->BindAction`
* Set it up in editor using a InputMappingContext to associate it to a key of your choice.

#### 5.5.2. Activation call

With that done, we can also expose a Gameplay Tag Container that will identify the abilities to trigger with that input:
```cpp
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess=true))
    FGameplayTagContainer PrimaryAbilityTags;
```

Alternatively, you could define the ability tag in a code file and use it directly. Look at GASamGameplayTags.h and .cpp for an example on how to create gameplay tags in code.

Then, in the function bound to the ability input (UsePrimaryAbility in my case), use the ASC's TryActivateAbilitiesByTag function:
```cpp
void AGasPlayerCharacter::UsePrimaryAbility(const FInputActionValue& Value)
{
	if (!AbilitySystemComponent)
	{
		return;
	}
	
	constexpr bool bAllowRemoteActivation = true;
	AbilitySystemComponent->TryActivateAbilitiesByTag(PrimaryAbilityTags, bAllowRemoteActivation);
}
```

#### 5.5.3. Setting up the data

The input will now try to activate abilities by the tags defined in your PrimaryAbilityTags container. In the editor, open your character blueprint and find this container in the Class Defaults. It'll show a box saying "Empty", click on it to create a GameplayTag with the name of your choice (I used GASam.Ability.Fireball). For more information on GameplayTags, you can look [here](https://github.com/tranek/GASDocumentation?tab=readme-ov-file#concepts-gt), they're quite powerful!

Then, we need to associate that tag with our fireball ability. Open the ability blueprint, select the class defaults, and add your newly created tag to the AssetTags (Default AbilityTags). When calling TryActivateAbilitiesByTag, the tag selected in AssetTags will be the one used to select the ability to activate. And that's it! The flow should go as follows:

* Somewhere in startup such as ACharacter::SetupPlayerInputComponent, an input action is bound to an UsePrimaryAbility function
* When the character is possessed, InitializeAbilitySystemComponent is called, granting the fireball ability
* When the player presses the correct input, UsePrimaryAbility is called and tries to activate abilities by the tags defined in PrimaryAbilityTags
* The fireball ability triggers, starting at the ActivateAbility event
* It spawns a new projectile using the SpawnProjectile function, then ends itself
* The player sees a projectile appear (with latency for clients) and fire off.

This is great and all, but there's still no interaction with the rest of the ability system. The projectile doesn't do any damage and doesn't cost anything to use. The next sections will address this.

#### 5.5.4. Applying damage

The projectile should take away a portion of a player's health if it lands on them. The simplest way to do this is with an instant gameplay effect, create one with a single modifier: one that "adds" -20 to the health attribute.

For a minimal working example: open your projectile and select the Collider component to implement its "On Component Begin Overlap" event. Here, you can start by skipping processing if the hit actor is the projectile's Instigator. Then, use "Get Ability System Component" on the hit actor. If it's not valid, as one would expect when the projectile hits the wall, simply have the projectile destroy itself. If it is valid, use the ASC's "ApplyGameplayEffectToSelf" node to apply your damage gameplay effect. Then, have the projectile destroy itself. This should be enough to have functional damage on your fireball, but it will be hardcoded and won't use the instigator's attributes.

In the example project in this repository, I set up the damage effect to use a "Set by caller" magnitude instead of hardcoding it to -20. The fireball gameplay ability prepares a Gameplay Effect Spec, assigning it a magnitude by tag (GASam.Damage), which the gameplay effect is set to read from. This makes it possible for the fireball ability to decide how much damage to apply, for example if the ability is leveled up or the instigating character has a buff.

The example project also makes use of the "Apply Additional Effect" component to apply a "On Fire" gameplay effect, which applies further damage over a period of time. Finally, it uses a "Damage" meta attribute instead of modifying health directly, more on this [here](https://github.com/tranek/GASDocumentation?tab=readme-ov-file#concepts-a-meta). You can also look at Unreal's Project Lyra, which handles damage in a similar way.

#### 5.5.5. Ability cost

It would be overpowered for a player to be able to use his fireball ability as much as he wants, and gameplay abilities have built-in ways to handle this: cost and cooldown.

Create a new set of attributes for "Mana" (and MaxMana) then update your Attribute Initializer effect (section 4.3) to fill up mana to the max. Alternatively, you could decide to use health as a currency for your abilities. Then, create a new gameplay effect to be used as the ability's cost. It should have a single modifier that "adds" a negative value of mana. Next, open up you fireball ability and find the "Cost Gameplay Effect Class" property in the Class Defaults. Select the new gameplay effect you made, and you should be good to go. 

If you try your ability at this point, you'll notice it doesn't apply the cost yet; this is because we need a final vital call to "CommitAbility". In the ability's event graph, connect a call to CommitAbility to the ActivateAbility event node. If it returns true, proceed with the spawning, otherwise call EndAbility. CommitAbility is the function that pays the cost you set as the Cost Gameplay Effect Class, if it can be paid. Your ability should now be consuming Mana, which you can confirm using the methods listed in section 3.4.

#### 5.5.6. Ability cooldown

Ability cooldowns are set up very similarly to ability costs. Create a gameplay effect for your cooldown, and change its Duration Policy to "Has Duration". This is where you'll set the cooldown's duration.

In the effect's Components, add "Grant Tags to Target Actor". Then, create a unique tag for this ability (or ability slot)'s cooldown, for example `Ability.Fireball.Cooldown` and select it.

Finally, open your GameplayAbility, find the "Cooldown Gameplay Effect Class" in the attribute defaults, and set it to your new Gameplay Effect. As with the ability cost, the cooldown won't be applied unless you have a call to "CommitAbility" after the gameplay activation node.