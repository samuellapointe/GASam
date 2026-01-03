// Samuel Lapointe - 2026

#include "GasPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GasPlayerState.h"

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
}

void AGasPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// The client's path for initializing the ASC. This is not necessary for single-player games.
	InitializeAbilitySystemComponent();
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
