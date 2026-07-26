
#include "ShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "FPS/Combat/CombatComponent.h"
#include "FPS/Data/WeaponData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

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
}

void AShooterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (IsValid(CombatComponent))
	{
		CombatComponent->SpawnInventory();
	}
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

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShooterCharacter::BeginDestroy()
{
	Super::BeginDestroy();
	
	if (IsValid(CombatComponent))
	{
		CombatComponent->DestroyInventory();
	}
}



