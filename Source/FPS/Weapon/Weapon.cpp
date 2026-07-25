
#include "Weapon.h"

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
	FirstPersonMesh->SetHiddenInGame(true);
	ThirdPersonMesh->SetupAttachment(FirstPersonMesh);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

