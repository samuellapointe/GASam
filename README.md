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
