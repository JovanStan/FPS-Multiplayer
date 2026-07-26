
#include "Weapon.h"

#include "FPS/Interfaces/PlayerInterface.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>("FirstPersonMesh");
	FirstPersonMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	FirstPersonMesh->bReceivesDecals = false;
	FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->SetHiddenInGame(true);
	
	SetRootComponent(FirstPersonMesh);
	
	ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ThirdPersonMesh");
	ThirdPersonMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	ThirdPersonMesh->bReceivesDecals = false;
	ThirdPersonMesh->CastShadow = true;
	ThirdPersonMesh->SetHiddenInGame(true);
	ThirdPersonMesh->SetupAttachment(FirstPersonMesh);
}

void AWeapon::OnRep_Instigator()
{
	Super::OnRep_Instigator();
	
	AttachToOwningPawn();
}

void AWeapon::AttachToOwningPawn() const
{
	APawn* OwningPawn = GetInstigator();
	if (!IsValid(OwningPawn) || !OwningPawn->Implements<UPlayerInterface>()) return;
	
	SetMeshVisibilities(OwningPawn);
	
	const FName AttachPoint = IPlayerInterface::Execute_GetWeaponAttachPoint(OwningPawn, WeaponType);
	USkeletalMeshComponent* FirstPersonMeshPawn = IPlayerInterface::Execute_GetFirstPersonMesh(OwningPawn);
	USkeletalMeshComponent* ThirdPersonMeshPawn = IPlayerInterface::Execute_GetThirdPersonMesh(OwningPawn);
	
	FirstPersonMesh->AttachToComponent(FirstPersonMeshPawn, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	ThirdPersonMesh->AttachToComponent(ThirdPersonMeshPawn, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::SetMeshVisibilities(const APawn* OwningPawn) const
{
	const bool bIsEquipped = OwningPawn->Implements<UPlayerInterface>() && IPlayerInterface::Execute_IsWeaponEquipped(OwningPawn, this);

	if (!bIsEquipped)
	{
		FirstPersonMesh->SetHiddenInGame(true);
		ThirdPersonMesh->SetHiddenInGame(true);
		return;
	}

	if (OwningPawn->IsLocallyControlled())
	{
		FirstPersonMesh->SetHiddenInGame(false);
		ThirdPersonMesh->SetHiddenInGame(true);
	}
	else
	{
		FirstPersonMesh->SetHiddenInGame(true);
		ThirdPersonMesh->SetHiddenInGame(false);
	}
}

