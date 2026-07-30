
#include "Weapon.h"

#include "CollisionShape.h"
#include "KismetTraceUtils.h"
#include "FPS/FPS.h"
#include "FPS/Interfaces/PlayerInterface.h"
#include "Kismet/KismetMathLibrary.h"

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
	
	AimFieldOfView = 65.f;
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

void AWeapon::WeaponTrace(FHitResult& HitResult, float TraceDistance)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(GetOwner());
	
	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_WorldStatic, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_WorldDynamic, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_PhysicsBody, ECR_Block);
	
	if (!GetInstigator()) return;
	if (APlayerController* PC = Cast<APlayerController>(GetInstigator()->GetController()))
	{
		FVector EyesWorldLocation;
		FRotator EyesWorldRotation;
		PC->GetActorEyesViewPoint(EyesWorldLocation, EyesWorldRotation);
		
		const FVector EyesWorldDirection = UKismetMathLibrary::GetForwardVector(EyesWorldRotation);
		const FVector StartLocation = EyesWorldLocation;
		const FVector EndLocation = StartLocation + EyesWorldDirection * TraceDistance;
		
		const bool bHit = GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, FPSTraceChannels::ECC_Weapon, 
			FCollisionShape::MakeSphere(5.f), QueryParams, ResponseParams);
		
		DrawDebugSphereTraceSingle(GetWorld(), StartLocation, EndLocation, 5.f, EDrawDebugTrace::ForDuration,
			bHit, HitResult, FColor::Green, FColor::Red, 5.f);
	}
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

