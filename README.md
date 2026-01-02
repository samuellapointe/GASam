# GASam

This is an example project meant to showcase a minimal setup of Unreal's Gameplay Ability System project with as least 
clutter as possible, along with some examples of abilities, effects and cues. It was made using the "Blank" project 
template in Unreal 5.6.1.

### Testing the game
Controls:
* Movement: WASD keys, or arrow keys
* Jump: Spacebar
* Camera: Mouse movement

### Browsing the code:
Throughout the project, useful information has been marked in comments with `[INFO]`

#### Suggested browsing order:
1. Initialization of the ASC
   1. [GasPlayerState.h](Source/GASam/Private/Character/Player/GasPlayerState.h), which is where the player's Ability 
      System lives.
   2. [AGasPlayerState::AGasPlayerState()](Source/GASam/Private/Character/Player/GasPlayerState.cpp) for the 
      instantiation
   3. [AGasPlayerCharacter::InitializeAbilitySystemComponent](Source/GASam/Private/Character/Player/GasPlayerCharacter.cpp) for the initialization