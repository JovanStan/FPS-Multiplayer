
#include "ShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "FPS/Combat/CombatComponent.h"
#include "FPS/Data/WeaponData.h"
#include "FPS/Weapon/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 0.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 15.f;
	SpringArm->bUsePawnControlRotation = true;
	
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("Camera");
	FirstPersonCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FirstPersonCamera->bUsePawnControlRotation = false;
	
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>("FirstPersonMesh");
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	FirstPersonMesh->bOnlyOwnerSee = true;
	FirstPersonMesh->bOwnerNoSee = false;
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->bReceivesDecals = false;
	FirstPersonMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	FirstPersonMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	
	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	
	GetCharacterMovement()->MovementState.bCanCrouch = true;
	
	CombatComponent = CreateDefaultSubobject<UCombatComponent>("Combat Component");
	CombatComponent->SetIsReplicated(true);
	
	DefaultFieldOfView = 90.f;
}

void AShooterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (IsValid(CombatComponent))
	{
		CombatComponent->SpawnInventory();
	}
}

void AShooterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	CalculateFabrikSocketTransform();
}

FName AShooterCharacter::GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const
{
	if (CombatComponent->WeaponData)
	{
		return CombatComponent->WeaponData->GripPoints.FindChecked(WeaponType);
	}
	return NAME_None;
}

USkeletalMeshComponent* AShooterCharacter::GetFirstPersonMesh_Implementation() const
{
	return FirstPersonMesh;
}

USkeletalMeshComponent* AShooterCharacter::GetThirdPersonMesh_Implementation() const
{
	return GetMesh();
}

bool AShooterCharacter::IsWeaponEquipped_Implementation(const AWeapon* Weapon) const
{
	return IsValid(CombatComponent) && CombatComponent->GetCurrentWeapon() == Weapon;
}

FRotator AShooterCharacter::GetFixedAimRotation() const
{
	FRotator AimRotation = GetBaseAimRotation();
	
	if (AimRotation.Pitch > 90.f && !IsLocallyControlled())
	{
		const FVector2D InRange(270.f, 360.f);
		const FVector2D OutRange(-90.f, 0.f);
		AimRotation.Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimRotation.Pitch);
	}
	return AimRotation;
}

void AShooterCharacter::CalculateFabrikSocketTransform()
{
	if (IsValid(CombatComponent) && IsValid(CombatComponent->GetCurrentWeapon()) && IsValid(CombatComponent->GetCurrentWeapon()->GetThirdPersonMesh()))
	{
		FABRIK_SocketTransform = CombatComponent->GetCurrentWeapon()->GetThirdPersonMesh()->GetSocketTransform("FABRIK_Socket", RTS_World);
		
		FVector OutLocation;
		FRotator OutRotation;
		GetMesh()->TransformToBoneSpace("hand_r", FABRIK_SocketTransform.GetLocation(), 
			FABRIK_SocketTransform.GetRotation().Rotator(), OutLocation, OutRotation);
		
		FABRIK_SocketTransform.SetLocation(OutLocation);
		FABRIK_SocketTransform.SetRotation(OutRotation.Quaternion());
	}
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	FirstPersonCamera->SetFieldOfView(DefaultFieldOfView);
}

void AShooterCharacter::BeginDestroy()
{
	Super::BeginDestroy();
	
	if (IsValid(CombatComponent))
	{
		CombatComponent->DestroyInventory();
	}
}



