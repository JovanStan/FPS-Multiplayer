
#include "CombatComponent.h"

#include "FPS/Data/WeaponData.h"
#include "FPS/Interfaces/PlayerInterface.h"
#include "FPS/Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, Inventory);
	DOREPLIFETIME(ThisClass, CurrentWeapon);
	DOREPLIFETIME_CONDITION(ThisClass, bAiming, COND_SkipOwner);
}

void UCombatComponent::Initiate_CycleWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Cycle Weapon");
}

void UCombatComponent::Initiate_FireWeapon_Pressed()
{
	Local_FireWeapon();
}

void UCombatComponent::Local_FireWeapon()
{
	if (!IsValid(WeaponData)) return;
	
	UAnimMontage* MontageFirstPerson = WeaponData->FirstPersonMontages.FindChecked(CurrentWeapon->WeaponType).FireMontage;
	USkeletalMeshComponent* MeshFirstPerson = IPlayerInterface::Execute_GetFirstPersonMesh(GetOwner());
	
	if (IsValid(MontageFirstPerson) && IsValid(MeshFirstPerson))
	{
		MeshFirstPerson->GetAnimInstance()->Montage_Play(MontageFirstPerson);
	}
	
	Server_FireWeapon();
}

void UCombatComponent::Server_FireWeapon_Implementation()
{
	Multicast_FireWeapon();
}

void UCombatComponent::Multicast_FireWeapon_Implementation()
{
	APawn* ControlledPawn = Cast<APawn>(GetOwner());
	if (ControlledPawn->IsLocallyControlled())
	{
		
	}
	else
	{
		if (!IsValid(WeaponData)) return;
	
		UAnimMontage* MontageThirdPerson = WeaponData->ThirdPersonMontages.FindChecked(CurrentWeapon->WeaponType).FireMontage;
		USkeletalMeshComponent* MeshThirdPerson = IPlayerInterface::Execute_GetThirdPersonMesh(GetOwner());
	
		if (IsValid(MontageThirdPerson) && IsValid(MeshThirdPerson))
		{
			MeshThirdPerson->GetAnimInstance()->Montage_Play(MontageThirdPerson);
		}
	}
}

void UCombatComponent::Initiate_FireWeapon_Released()
{
	
}

void UCombatComponent::Initiate_ReloadWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, "Reload Weapon");
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

// This is for local player
void UCombatComponent::Local_Aim(bool bPressed)
{
	bAiming = bPressed;
}

// This is for server to tell the rest of the clients that we are aiming
void UCombatComponent::Server_Aim_Implementation(bool bPressed)
{
	Local_Aim(bPressed);
}

void UCombatComponent::Equip(AWeapon* Weapon)
{
	CurrentWeapon = Weapon;
	CurrentWeapon->AttachToOwningPawn();
}

void UCombatComponent::SpawnInventory()
{
	if (GetOwner()->GetLocalRole() != ROLE_Authority) return;
	
	for (const TSubclassOf<AWeapon>& WeaponClass : DefaultWeaponClasses)
	{
		AWeapon* Weapon = SpawnWeapon(WeaponClass);
		Inventory.AddUnique(Weapon);
	}
	
	if (Inventory.Num() > 0)
	{
		Equip(Inventory[0]);
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

void UCombatComponent::OnRep_CurrentWeapon(AWeapon* LastWeapon)
{
	if (!IsValid(CurrentWeapon)) return;
	CurrentWeapon->AttachToOwningPawn(); 
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


