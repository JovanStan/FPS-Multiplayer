
#include "ShooterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "FPS/Characters/ShooterCharacter.h"
#include "FPS/Combat/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AShooterPlayerController::AShooterPlayerController()
{
	bReplicates = true;
}


void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(ShooterContext, 0);
	}
	
	CachedShooterCharacter = Cast<AShooterCharacter>(GetCharacter());
}


void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::Input_Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::Input_Look);
	
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AShooterPlayerController::Input_Jump);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AShooterPlayerController::Input_Crouch);
	
	EnhancedInputComponent->BindAction(CycleWeaponAction, ETriggerEvent::Started, this, &AShooterPlayerController::Input_CycleWeapon);
	EnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &AShooterPlayerController::Input_FireWeapon_Pressed);
	EnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Completed, this, &AShooterPlayerController::Input_FireWeapon_Released);
	EnhancedInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Started, this, &AShooterPlayerController::Input_Aim_Pressed);
	EnhancedInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Completed, this, &AShooterPlayerController::Input_Aim_Released);
	EnhancedInputComponent->BindAction(ReloadWeaponAction, ETriggerEvent::Started, this, &AShooterPlayerController::Input_ReloadWeapon);
}

void AShooterPlayerController::Input_Crouch()
{
	if (!IsValid(GetCharacter())) return;
	
	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacter()->GetCharacterMovement())
	{
		CharacterMovementComponent->bWantsToCrouch = !CharacterMovementComponent->bWantsToCrouch;
	}
}

void AShooterPlayerController::Input_Jump()
{
	if (!IsValid(GetCharacter())) return;
	
	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacter()->GetCharacterMovement())
	{
		if (CharacterMovementComponent->bWantsToCrouch)
		{
			CharacterMovementComponent->bWantsToCrouch = false;
		}
		else
		{
			GetCharacter()->Jump();
		}
	}
}

void AShooterPlayerController::Input_Move(const FInputActionValue& Value)
{
	const FVector2D InputAxisVector = Value.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if (APawn* ControlledPawn = GetPawn())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AShooterPlayerController::Input_Look(const FInputActionValue& Value)
{
	const FVector2D InputAxisVector = Value.Get<FVector2D>();
	
	AddYawInput(InputAxisVector.X);
	AddPitchInput(InputAxisVector.Y);
}

void AShooterPlayerController::Input_CycleWeapon()
{
	if (CachedShooterCharacter->GetCombatComponent())
	{
		CachedShooterCharacter->GetCombatComponent()->Initiate_CycleWeapon();
	}
}

void AShooterPlayerController::Input_ReloadWeapon()
{
	if (CachedShooterCharacter->GetCombatComponent())
	{
		CachedShooterCharacter->GetCombatComponent()->Initiate_ReloadWeapon();
	}
}

void AShooterPlayerController::Input_FireWeapon_Pressed()
{
	if (CachedShooterCharacter->GetCombatComponent())
	{
		CachedShooterCharacter->GetCombatComponent()->Initiate_FireWeapon_Pressed();
	}
}

void AShooterPlayerController::Input_FireWeapon_Released()
{
	if (CachedShooterCharacter->GetCombatComponent())
	{
		CachedShooterCharacter->GetCombatComponent()->Initiate_FireWeapon_Released();
	}
}

void AShooterPlayerController::Input_Aim_Pressed()
{
	if (CachedShooterCharacter->GetCombatComponent())
	{
		CachedShooterCharacter->GetCombatComponent()->Initiate_Aim_Pressed();
		CachedShooterCharacter->OnAim(true);
	}
}

void AShooterPlayerController::Input_Aim_Released()
{
	if (CachedShooterCharacter->GetCombatComponent())
	{
		CachedShooterCharacter->GetCombatComponent()->Initiate_Aim_Released();
		CachedShooterCharacter->OnAim(false);
	}
}
