
#include "ShooterReticle.h"

#include "../Characters/ShooterCharacter.h"
#include "Components/Image.h"
#include "FPS/Combat/CombatComponent.h"
#include "FPS/Weapon/Weapon.h"

namespace Ammo
{
	const FName Rounds_Current = FName("Rounds_Current");
	const FName Rounds_Max = FName("Rounds_Max");
}

void UShooterReticle::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ReticleImage->SetRenderOpacity(0.0f);
	AmmoCounterImage->SetRenderOpacity(0.0f);
	
	GetOwningPlayer()->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
	
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;
	
	OnPossessedPawnChanged(nullptr, ShooterCharacter);
	
	if (ShooterCharacter->HasWeaponFirstReplicated())
	{
		AWeapon* Weapon = IPlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter);
		if (IsValid(Weapon))
		{
			OnReticleChanged(Weapon->GetReticleDynamic());
			OnAmmoCounterChanged(Weapon->GetAmmoCounterDynamic(), Weapon->Ammo, Weapon->MagCapacity);
		}
	}
	else
	{
		ShooterCharacter->OnWeaponFirstReplicated.AddDynamic(this, &ThisClass::OnWeaponFirstReplicated);
	}
	
	if (ShooterCharacter->HasAuthority())
	{
		AWeapon* Weapon = IPlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter);
		if (!IsValid(Weapon)) return;
		
		OnReticleChanged(Weapon->GetReticleDynamic());
		OnAmmoCounterChanged(Weapon->GetAmmoCounterDynamic(), Weapon->Ammo, Weapon->MagCapacity);
	}
}

void UShooterReticle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UShooterReticle::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	UCombatComponent* OldPawnCombat = UCombatComponent::FindCombatComponent(OldPawn);
	if (IsValid(OldPawnCombat))
	{
		OldPawnCombat->OnReticleChanged.RemoveDynamic(this, &ThisClass::OnReticleChanged);
		OldPawnCombat->OnAmmoCounterChanged.RemoveDynamic(this, &ThisClass::OnAmmoCounterChanged);
		OldPawnCombat->OnRoundFired.RemoveDynamic(this, &ThisClass::OnRoundFired);
	}
	
	UCombatComponent* NewPawnCombat = UCombatComponent::FindCombatComponent(NewPawn);
	if (IsValid(NewPawnCombat))
	{
		ReticleImage->SetRenderOpacity(1.0f);
		AmmoCounterImage->SetRenderOpacity(1.0f);
		NewPawnCombat->OnReticleChanged.AddDynamic(this, &ThisClass::OnReticleChanged);
		NewPawnCombat->OnAmmoCounterChanged.AddDynamic(this, &ThisClass::OnAmmoCounterChanged);
		NewPawnCombat->OnRoundFired.AddDynamic(this, &ThisClass::OnRoundFired);
	}
}

void UShooterReticle::OnWeaponFirstReplicated(AWeapon* Weapon)
{
	OnReticleChanged(Weapon->GetReticleDynamic());
	OnAmmoCounterChanged(Weapon->GetAmmoCounterDynamic(), Weapon->Ammo, Weapon->MagCapacity);
}

void UShooterReticle::OnReticleChanged(UMaterialInstanceDynamic* ReticleDynamic)
{
	CurrentReticle = ReticleDynamic;
	
	FSlateBrush Brush;
	Brush.SetResourceObject(ReticleDynamic);
	if (IsValid(ReticleImage))
	{
		ReticleImage->SetBrush(Brush);
	}
}

void UShooterReticle::OnAmmoCounterChanged(UMaterialInstanceDynamic* AmmoCounterDynamic, int RoundsCurrent,int RoundsMax)
{
	CurrentAmmoCounter = AmmoCounterDynamic;
	CurrentAmmoCounter->SetScalarParameterValue(Ammo::Rounds_Current, RoundsCurrent);
	CurrentAmmoCounter->SetScalarParameterValue(Ammo::Rounds_Max, RoundsMax);
	
	FSlateBrush Brush;
	Brush.SetResourceObject(AmmoCounterDynamic);
	if (IsValid(AmmoCounterImage))
	{
		AmmoCounterImage->SetBrush(Brush);
	}
}

void UShooterReticle::OnRoundFired(int RoundsCurrent, int RoundsMax)
{
	if (CurrentAmmoCounter.IsValid())
	{
		CurrentAmmoCounter->SetScalarParameterValue(Ammo::Rounds_Current, RoundsCurrent);
		CurrentAmmoCounter->SetScalarParameterValue(Ammo::Rounds_Max, RoundsMax);
	}
}
