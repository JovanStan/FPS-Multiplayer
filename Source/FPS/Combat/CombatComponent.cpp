
#include "CombatComponent.h"

#include "Chaos/ChaosEngineInterface.h"
#include "FPS/Data/WeaponData.h"
#include "FPS/Interfaces/PlayerInterface.h"
#include "FPS/Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	Local_WeaponIndex = 0;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, Inventory);
	DOREPLIFETIME(ThisClass, CurrentWeapon);
	DOREPLIFETIME_CONDITION(ThisClass, bAiming, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ThisClass, CurrentReserveAmmo, COND_OwnerOnly);
}

void UCombatComponent::Initiate_CycleWeapon()
{
	if (!IsValid(CurrentWeapon)) return;
	if (CurrentWeapon->WeaponStatus == EWeaponStatus::Cycling) return;
	
	AdvanceWeaponIndex();
	Local_CycleWeapon(Local_WeaponIndex);
}

void UCombatComponent::Local_CycleWeapon(int32 WeaponIndex)
{
	AWeapon* NextWeapon = Inventory[WeaponIndex];
	if (!IsValid(NextWeapon) || !IsValid(WeaponData)) return;
	CurrentWeapon->WeaponStatus = EWeaponStatus::Cycling;
	NextWeapon->WeaponStatus = EWeaponStatus::Cycling;
	
	APawn* OwningPawn = Cast<APawn>(GetOwner());
	const bool bIsLocal = IsValid(OwningPawn) && OwningPawn->IsLocallyControlled();
	
	const FMontageData MontageData = bIsLocal ? 
	WeaponData->FirstPersonMontages.FindChecked(NextWeapon->WeaponType) 
	: WeaponData->ThirdPersonMontages.FindChecked(NextWeapon->WeaponType);
	
	USkeletalMeshComponent* Mesh = bIsLocal ? IPlayerInterface::Execute_GetFirstPersonMesh(GetOwner()) : IPlayerInterface::Execute_GetThirdPersonMesh(GetOwner());
	
	if (IsValid(Mesh) && IsValid(MontageData.EquipMontage))
	{
		Mesh->GetAnimInstance()->Montage_Play(MontageData.EquipMontage);
	}
	
	if (bIsLocal)
	{
		Server_CycleWeapon(WeaponIndex);
		Mesh->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, & ThisClass::BlendOut_CycleWeapon);
	}
}

void UCombatComponent::Server_CycleWeapon_Implementation(int32 WeaponIndex)
{
	Local_WeaponIndex = WeaponIndex;
	MultiCast_CycleWeapon(WeaponIndex);
}

void UCombatComponent::MultiCast_CycleWeapon_Implementation(int32 WeaponIndex)
{
	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (!IsValid(OwningPawn)) return;
	
	if (!OwningPawn->IsLocallyControlled())
	{
		Local_WeaponIndex = WeaponIndex;
		Local_CycleWeapon(WeaponIndex);
	}
}

void UCombatComponent::Notify_CycleWeapon()
{
	if (!IsValid(CurrentWeapon)) return;
	
	AWeapon* NewWeapon = Inventory[Local_WeaponIndex];
	if (IsValid(NewWeapon))
	{
		EquipWeapon(NewWeapon);
	}
}

void UCombatComponent::BlendOut_CycleWeapon(UAnimMontage* Montage, bool bInterrupted)
{
	if (UAnimInstance* AnimInstance = IPlayerInterface::Execute_GetFirstPersonMesh(GetOwner())->GetAnimInstance())
	{
		if (AnimInstance->OnMontageBlendingOut.IsAlreadyBound(this, &ThisClass::BlendOut_CycleWeapon))
		{
			AnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &ThisClass::BlendOut_CycleWeapon);
		}
	}
	CurrentWeapon->WeaponStatus = EWeaponStatus::Idle;
	
	OnReticleChanged.Broadcast(CurrentWeapon->GetReticleDynamic(), CurrentWeapon->ReticleParams);
	OnAmmoCounterChanged.Broadcast(CurrentWeapon->GetAmmoCounterDynamic(), CurrentWeapon->Ammo, CurrentWeapon->MagCapacity);
	OnCurrentReserveAmmoChanged.Broadcast(CurrentReserveAmmo, CurrentWeapon->Ammo, CurrentWeapon->WeaponIcon);
	
	if (bTriggerPressed && CurrentWeapon->FireType == EFireType::Automatic && CurrentWeapon->Ammo > 0)
	{
		Local_FireWeapon();
	}
}

void UCombatComponent::Initiate_FireWeapon_Pressed()
{
	if (!IsValid(CurrentWeapon)) return;
	
	bTriggerPressed = true;
	
	if (CurrentWeapon->WeaponStatus == EWeaponStatus::Idle && CurrentWeapon->Ammo > 0)
	{
		Local_FireWeapon();
	}
	else
	{
		CurrentWeapon->DryFire();
	}
}

void UCombatComponent::Local_FireWeapon()
{
	// All this is done Locally
	if (!IsValid(CurrentWeapon)) return;
	if (!IsValid(WeaponData)) return;
	
	CurrentWeapon->WeaponStatus = EWeaponStatus::Firing;
	
	UAnimMontage* MontageFirstPerson = WeaponData->FirstPersonMontages.FindChecked(CurrentWeapon->WeaponType).FireMontage;
	USkeletalMeshComponent* MeshFirstPerson = IPlayerInterface::Execute_GetFirstPersonMesh(GetOwner());
	
	if (IsValid(MontageFirstPerson) && IsValid(MeshFirstPerson))
	{
		MeshFirstPerson->GetAnimInstance()->Montage_Play(MontageFirstPerson);
	}
	FHitResult HitResult;
	EPhysicalSurface ImpactSurfaceType = HitResult.PhysMaterial.IsValid(false) ? HitResult.PhysMaterial->SurfaceType.GetValue() : SurfaceType1;
	
	CurrentWeapon->WeaponTrace(HitResult, TraceDistance);
	CurrentWeapon->Local_Fire(HitResult.ImpactPoint, HitResult.ImpactNormal, ImpactSurfaceType, true);
	OnRoundFired.Broadcast(CurrentWeapon->Ammo, CurrentWeapon->MagCapacity, CurrentReserveAmmo);
	
	GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &ThisClass::FireTimerFinished, CurrentWeapon->FireTime);
	
	// Tell Server that we are firing
	Server_FireWeapon(HitResult);
}


void UCombatComponent::Server_FireWeapon_Implementation(const FHitResult& HitResult)
{
	// Server then MultiCast to all other clients that we are firing
	if (!IsValid(CurrentWeapon)) return;
	
	if (CurrentWeapon->Ammo <= 0) return;
	
	if (GetNetMode() != NM_ListenServer || !Cast<APawn>(GetOwner())->IsLocallyControlled())
	{
		CurrentWeapon->Auth_Fire();
	}
	Multicast_FireWeapon(HitResult, CurrentWeapon->Ammo);
}

void UCombatComponent::Multicast_FireWeapon_Implementation(const FHitResult& HitResult, int AuthAmmo)
{
	APawn* ControlledPawn = Cast<APawn>(GetOwner());
	if (ControlledPawn->IsLocallyControlled())
	{
		CurrentWeapon->Rep_Fire(AuthAmmo);
	}
	else
	{
		if (!IsValid(WeaponData)) return;
		
		EPhysicalSurface ImpactSurfaceType = HitResult.PhysMaterial.IsValid(false) ? HitResult.PhysMaterial->SurfaceType.GetValue() : SurfaceType1;
		CurrentWeapon->Local_Fire(HitResult.ImpactPoint, HitResult.ImpactNormal, ImpactSurfaceType, false);
	
		UAnimMontage* MontageThirdPerson = WeaponData->ThirdPersonMontages.FindChecked(CurrentWeapon->WeaponType).FireMontage;
		USkeletalMeshComponent* MeshThirdPerson = IPlayerInterface::Execute_GetThirdPersonMesh(GetOwner());
	
		if (IsValid(MontageThirdPerson) && IsValid(MeshThirdPerson))
		{
			MeshThirdPerson->GetAnimInstance()->Montage_Play(MontageThirdPerson);
		}
	}
}

void UCombatComponent::FireTimerFinished()
{
	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (!IsValid(CurrentWeapon) || !IsValid(OwningPawn)) return;
	
	if (CurrentWeapon->Ammo == 0 && CurrentReserveAmmo > 0 && OwningPawn->IsLocallyControlled())
	{
		Local_ReloadWeapon();
		Server_ReloadWeapon();
		return;
	} 
	
	if (CurrentWeapon->WeaponStatus == EWeaponStatus::Firing)
	{
		CurrentWeapon->WeaponStatus = EWeaponStatus::Idle;
	}
	
	if (bTriggerPressed && CurrentWeapon->FireType == Automatic && CurrentWeapon->Ammo > 0)
	{
		Local_FireWeapon();
	}
}

int32 UCombatComponent::AdvanceWeaponIndex()
{
	if (Inventory.Num() >= 2)
	{
		Local_WeaponIndex = (Local_WeaponIndex + 1) % Inventory.Num();
	}
	return Local_WeaponIndex;
}


void UCombatComponent::Initiate_FireWeapon_Released()
{
	bTriggerPressed = false;
}

void UCombatComponent::Initiate_ReloadWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, "Reload Weapon");
	
	if (!IsValid(CurrentWeapon)) return;
	if (CurrentWeapon->WeaponStatus == EWeaponStatus::Cycling || CurrentWeapon->WeaponStatus == EWeaponStatus::Reloading) return;
	if (CurrentWeapon->Ammo == CurrentWeapon->MagCapacity) return;
	if (CurrentReserveAmmo == 0) return;
	
	Local_ReloadWeapon();
	Server_ReloadWeapon();
}

void UCombatComponent::Local_ReloadWeapon()
{
	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (!IsValid(CurrentWeapon) || !IsValid(OwningPawn)) return;
	
	const bool bIsLocal = OwningPawn->IsLocallyControlled();
	
	UAnimMontage* ReloadMontage = bIsLocal ? 
	WeaponData->FirstPersonMontages.FindChecked(CurrentWeapon->WeaponType).ReloadMontage :
	WeaponData->ThirdPersonMontages.FindChecked(CurrentWeapon->WeaponType).ReloadMontage;
	const USkeletalMeshComponent* Mesh = bIsLocal ? IPlayerInterface::Execute_GetFirstPersonMesh(OwningPawn) : IPlayerInterface::Execute_GetThirdPersonMesh(OwningPawn);
	
	if (IsValid(ReloadMontage) && IsValid(Mesh))
	{
		Mesh->GetAnimInstance()->Montage_Play(ReloadMontage);
	}
	
	UAnimMontage* WeaponReloadMontage = WeaponData->WeaponMontages.FindChecked(CurrentWeapon->WeaponType).ReloadMontage;
	USkeletalMeshComponent* WeaponMesh = bIsLocal ? CurrentWeapon->GetFirstPersonMesh() : CurrentWeapon->GetThirdPersonMesh();
	
	if (IsValid(WeaponReloadMontage) && IsValid(WeaponMesh))
	{
		WeaponMesh->GetAnimInstance()->Montage_Play(WeaponReloadMontage);
	}
	CurrentWeapon->WeaponStatus = EWeaponStatus::Reloading;
}


void UCombatComponent::Server_ReloadWeapon_Implementation()
{
	Multicast_ReloadWeapon();
}

void UCombatComponent::Multicast_ReloadWeapon_Implementation()
{
	Local_ReloadWeapon();
}


void UCombatComponent::Initiate_Aim_Pressed()
{
	Local_Aim(true);
	Server_Aim(true);
}

void UCombatComponent::Initiate_Aim_Released()
{
	Local_Aim(false);
	Server_Aim(false);
}

void UCombatComponent::AddAmmo(const FGameplayTag& WeaponType, int32 Amount)
{
	if (GetOwner()->HasAuthority() && !IsValid(CurrentWeapon)) return;
	
	if (!ReserveAmmo.Contains(WeaponType))
	{
		ReserveAmmo.Add(WeaponType, Amount);1
	}
	else
	{
		const int32 NewAmmo = ReserveAmmo.FindChecked(WeaponType) + Amount;
		ReserveAmmo[WeaponType] = NewAmmo;
		
		if (CurrentWeapon->WeaponType.MatchesTagExact(WeaponType))
		{
			CurrentReserveAmmo = NewAmmo;
			if (CurrentWeapon->Ammo == 0 && NewAmmo > 0)
			{
				Server_ReloadWeapon();
			}
			OnAmmoCounterChanged.Broadcast(CurrentWeapon->GetAmmoCounterDynamic(), CurrentWeapon->Ammo, CurrentWeapon->MagCapacity);
			OnCurrentReserveAmmoChanged.Broadcast(CurrentReserveAmmo, CurrentWeapon->Ammo, CurrentWeapon->WeaponIcon);
		}
	}
}

void UCombatComponent::Notify_ReloadWeapon()
{
	if (!IsValid(CurrentWeapon)) return;
	
	if (GetNetMode() == NM_ListenServer || GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_Standalone)
	{
		const int32 EmptySpace = CurrentWeapon->MagCapacity - CurrentWeapon->Ammo;
		const int32 AmountToRefill = FMath::Min(EmptySpace, CurrentReserveAmmo);
		CurrentWeapon->Ammo += AmountToRefill;
		ReserveAmmo[CurrentWeapon->WeaponType] = ReserveAmmo[CurrentWeapon->WeaponType] - AmountToRefill;
		CurrentReserveAmmo = ReserveAmmo[CurrentWeapon->WeaponType];
		Client_ReloadWeapon(CurrentWeapon->Ammo, CurrentReserveAmmo);
	}
	CurrentWeapon->WeaponStatus = EWeaponStatus::Idle;
	if (bTriggerPressed && CurrentWeapon->Ammo > 0)
	{
		Local_FireWeapon();
	}
}

void UCombatComponent::Client_ReloadWeapon_Implementation(int32 NewWeaponAmmo, int32 NewCarriedAmmo)
{
	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (!IsValid(CurrentWeapon) || !IsValid(OwningPawn)) return;
	
	if (OwningPawn->IsLocallyControlled())
	{
		CurrentWeapon->Ammo = NewWeaponAmmo;
		CurrentReserveAmmo = NewCarriedAmmo;
		
		OnAmmoCounterChanged.Broadcast(CurrentWeapon->GetAmmoCounterDynamic(), CurrentWeapon->Ammo, CurrentWeapon->MagCapacity);
		OnCurrentReserveAmmoChanged.Broadcast(CurrentReserveAmmo, CurrentWeapon->Ammo, CurrentWeapon->WeaponIcon);
	}
}

// This is for local player
void UCombatComponent::Local_Aim(bool bPressed)
{
	bAiming = bPressed;
	OnAimingStatusChanged.Broadcast(bAiming);
}

// This is for server to tell the rest of the clients that we are aiming
void UCombatComponent::Server_Aim_Implementation(bool bPressed)
{
	Local_Aim(bPressed);
}

void UCombatComponent::Equip(AWeapon* Weapon)
{
	CurrentWeapon = Weapon;
	CurrentWeapon->AttachToOwningPawn(Cast<APawn>(GetOwner()));
	
	CurrentReserveAmmo = ReserveAmmo.FindChecked(CurrentWeapon->WeaponType);
	OnCurrentReserveAmmoChanged.Broadcast(CurrentReserveAmmo, Weapon->Ammo, CurrentWeapon->WeaponIcon);
}

void UCombatComponent::EquipWeapon(AWeapon* Weapon)
{
	if (!IsValid(Weapon) || !IsValid(GetOwner())) return;
	
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		SetCurrentWeapon(Weapon, CurrentWeapon);
	}
	else
	{
		Server_EquipWeapon(Weapon);
	}
}

void UCombatComponent::Server_EquipWeapon_Implementation(AWeapon* Weapon)
{
	EquipWeapon(Weapon);
}

void UCombatComponent::SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon)
{
	AWeapon* LocalLastWeapon = nullptr;
	
	if (IsValid(LastWeapon))
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}
	
	if (IsValid(LocalLastWeapon))
	{
		LocalLastWeapon->DetachFromOwningPawn();
		LocalLastWeapon->WeaponStatus = EWeaponStatus::Unequipped;
	}
	
	CurrentWeapon = NewWeapon;
	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (IsValid(OwningPawn) && OwningPawn->HasAuthority() && IsValid(CurrentWeapon))
	{
		CurrentReserveAmmo = ReserveAmmo.FindChecked(CurrentWeapon->WeaponType);
	}
	if (!IsValid(CurrentWeapon)) return;
	CurrentWeapon->AttachToOwningPawn(OwningPawn);
	
	if (CurrentWeapon->Ammo == 0 && CurrentReserveAmmo > 0 && OwningPawn->IsLocallyControlled())
	{
		Local_ReloadWeapon();
		Server_ReloadWeapon();
	}
}

void UCombatComponent::SpawnInventory()
{
	if (GetOwner()->GetLocalRole() != ROLE_Authority) return;
	
	for (const TSubclassOf<AWeapon>& WeaponClass : DefaultWeaponClasses)
	{
		AWeapon* Weapon = SpawnWeapon(WeaponClass);
		Inventory.AddUnique(Weapon);
		ReserveAmmo.Add(Weapon->WeaponType, Weapon->StartingCarriedAmmo);
	}
	
	if (Inventory.Num() > 0)
	{
		Equip(Inventory[0]);
		InitializeWeaponWidgets();
	}
}

void UCombatComponent::DestroyInventory()
{
	for (AWeapon* Weapon : Inventory)
	{
		if (IsValid(Weapon))
		{
			Weapon->Destroy();
		}
	}
}

void UCombatComponent::InitializeWeaponWidgets()
{
	if (IsValid(CurrentWeapon))
	{
		OnReticleChanged.Broadcast(CurrentWeapon->GetReticleDynamic(), CurrentWeapon->ReticleParams);
		OnAmmoCounterChanged.Broadcast(CurrentWeapon->GetAmmoCounterDynamic(), CurrentWeapon->Ammo, CurrentWeapon->MagCapacity);
	}
}

void UCombatComponent::OnRep_CurrentReserveAmmo()
{
	if (IsValid(CurrentWeapon))
	{
		OnCurrentReserveAmmoChanged.Broadcast(CurrentReserveAmmo, CurrentWeapon->Ammo, CurrentWeapon->WeaponIcon);
	}
}

void UCombatComponent::OnRep_CurrentWeapon(AWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
	
	IPlayerInterface::Execute_WeaponReplicated(GetOwner());
	InitializeWeaponWidgets();
}

AWeapon* UCombatComponent::SpawnWeapon(TSubclassOf<AWeapon> WeaponClass)
{
	AActor* OwningActor = GetOwner();
	if (!IsValid(OwningActor)) return nullptr;
	if (OwningActor->GetLocalRole() < ROLE_Authority) return nullptr;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Cast<APawn>(OwningActor);
	SpawnParams.Owner = OwningActor;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	return GetWorld()->SpawnActor<AWeapon>(WeaponClass, SpawnParams);
}


