// Samuel Lapointe - 2026

#include "GasPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GASamGameplayTags.h"
#include "GasPlayerState.h"
#include "UI/GasHUD.h"

void AGasPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AGasPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	check(EnhancedInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGasPlayerCharacter::Move);

	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGasPlayerCharacter::Look);
	
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	/* [INFO] Abilities can be bound directly to inputs by defining an enum of Input IDs and using FGameplayAbilityInputBinds.
	 * However, Epic mentions they don't use this in any of their shipped titles due to inflexibility. As an alternative,
	 * we use TryActivateAbilityByTag and we let design decide which ability is triggered by the primary and secondary
	 * ability inputs.
	 */
	EnhancedInputComponent->BindAction(PrimaryAbilityAction, ETriggerEvent::Triggered, this, &AGasPlayerCharacter::UsePrimaryAbility);
	EnhancedInputComponent->BindAction(SecondaryAbilityAction, ETriggerEvent::Triggered, this, &AGasPlayerCharacter::UseSecondaryAbility);
}

UAbilitySystemComponent* AGasPlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGasPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// The server's path for initializing the ASC
	InitializeAbilitySystemComponent();
	InitializeHUD();
}

void AGasPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// The client's path for initializing the ASC. This is not necessary for single-player games.
	InitializeAbilitySystemComponent();
	InitializeHUD();
}

void AGasPlayerCharacter::InitializeAbilitySystemComponent()
{
	/* [INFO] This is a function called both by the server and the client to initialize the AbilitySystemComponent
	 * when we know it's accessible. Calling InitAbilityActorInfo sets the OwnerActor, on whose lifetime the ASC
	 * depends, and the AvatarActor, which is the physical representation of the ASC's owner in the game world.
	 *
	 * InitAbilityActorInfo must be called on both the server and the client, and it should be called whenever the
	 * owner or avatar actor changes. 
	 * 
	 * Calling InitAbilityActorInfo multiple times is fine, and having no AvatarActor is also technically fine,
	 * for example to have some attributes or effects persist while your character is respawning.
	 */
	AGasPlayerState* GasPlayerState = GetPlayerStateChecked<AGasPlayerState>();

	AbilitySystemComponent = GasPlayerState->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(GasPlayerState, this);

	if (HasAuthority())
	{
		GasPlayerState->GrantDefaultAbilities();
		GasPlayerState->ApplyDefaultEffects(); // Serves both to initialize all attributes (health, mana) and refill them on respawn
	}
}

void AGasPlayerCharacter::InitializeHUD() const
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (AGasHUD* GasHUD = Cast<AGasHUD>(PlayerController->GetHUD()))
		{
			AGasPlayerState* GasPlayerState = GetPlayerStateChecked<AGasPlayerState>();
			GasHUD->InitOverlay(GasPlayerState);
		}
	}
}

void AGasPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AGasPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGasPlayerCharacter::UsePrimaryAbility(const FInputActionValue& Value)
{
	UseAbilityByTags(PrimaryAbilityTags);
}

void AGasPlayerCharacter::UseSecondaryAbility(const FInputActionValue& Value)
{
	UseAbilityByTags(SecondaryAbilityTags);
}

void AGasPlayerCharacter::UseAbilityByTags(const FGameplayTagContainer& AbilityTags) const
{
	if (!AbilitySystemComponent)
	{
		return;
	}
	
	constexpr bool bAllowRemoteActivation = true;
	AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
}
