
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

namespace Reticle
{
	const FName RoundedCornerScale = FName("RoundedCornerScale");
	const FName ShapeCutThickness = FName("ShapeCutThickness");
}

void UShooterReticle::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ReticleImage->SetRenderOpacity(0.0f);
	AmmoCounterImage->SetRenderOpacity(0.0f);
	_BaseCornerScaleFactor_RoundFired = 0.f;
	_BaseShapeCutFactor_RoundFired = 0.f;
	_BaseCornerScaleFactor_Aiming = 0.f;
	_BaseShapeCutFactor_Aiming = 0.f;
	bAiming = false;
	
	GetOwningPlayer()->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
	
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;
	
	OnPossessedPawnChanged(nullptr, ShooterCharacter);
	
	if (ShooterCharacter->HasWeaponFirstReplicated())
	{
		AWeapon* Weapon = IPlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter);
		if (IsValid(Weapon))
		{
			OnReticleChanged(Weapon->GetReticleDynamic(), Weapon->ReticleParams);
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
		
		OnReticleChanged(Weapon->GetReticleDynamic(), Weapon->ReticleParams);
		OnAmmoCounterChanged(Weapon->GetAmmoCounterDynamic(), Weapon->Ammo, Weapon->MagCapacity);
	}
}

void UShooterReticle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	_BaseCornerScaleFactor_RoundFired = FMath::FInterpTo(_BaseCornerScaleFactor_RoundFired, 0.f, InDeltaTime, CurrentReticleParams.RoundFiredInterpSpeed);
	_BaseShapeCutFactor_RoundFired = FMath::FInterpTo(_BaseShapeCutFactor_RoundFired, 0.f, InDeltaTime, CurrentReticleParams.RoundFiredInterpSpeed);
	
	_BaseCornerScaleFactor_Aiming = FMath::FInterpTo(_BaseCornerScaleFactor_Aiming, 
		bAiming ? CurrentReticleParams.ScaleFactor_Aiming : CurrentReticleParams.ScaleFactor_NotAiming,
		InDeltaTime, CurrentReticleParams.AimingInterpSpeed);
	
	_BaseShapeCutFactor_Aiming = FMath::FInterpTo(_BaseShapeCutFactor_Aiming,
		bAiming ? CurrentReticleParams.ShapeCutFactor_Aiming : CurrentReticleParams.ShapeCutFactor_NotAiming, 
		InDeltaTime, CurrentReticleParams.AimingInterpSpeed);
	
	BaseCornerScaleFactor = _BaseCornerScaleFactor_RoundFired + _BaseCornerScaleFactor_Aiming;
	BaseShapeCutFactor = _BaseShapeCutFactor_RoundFired + _BaseShapeCutFactor_Aiming;
	
	if (CurrentReticle.IsValid())
	{
		CurrentReticle->SetScalarParameterValue(Reticle::RoundedCornerScale, BaseCornerScaleFactor);
		CurrentReticle->SetScalarParameterValue(Reticle::ShapeCutThickness, BaseShapeCutFactor);
	}
}

void UShooterReticle::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	UCombatComponent* OldPawnCombat = UCombatComponent::FindCombatComponent(OldPawn);
	if (IsValid(OldPawnCombat))
	{
		OldPawnCombat->OnReticleChanged.RemoveDynamic(this, &ThisClass::OnReticleChanged);
		OldPawnCombat->OnAmmoCounterChanged.RemoveDynamic(this, &ThisClass::OnAmmoCounterChanged);
		OldPawnCombat->OnRoundFired.RemoveDynamic(this, &ThisClass::OnRoundFired);
		OldPawnCombat->OnAimingStatusChanged.RemoveDynamic(this, &ThisClass::OnAimingStatusChanged);
	}
	
	UCombatComponent* NewPawnCombat = UCombatComponent::FindCombatComponent(NewPawn);
	if (IsValid(NewPawnCombat))
	{
		ReticleImage->SetRenderOpacity(1.0f);
		AmmoCounterImage->SetRenderOpacity(1.0f);
		NewPawnCombat->OnReticleChanged.AddDynamic(this, &ThisClass::OnReticleChanged);
		NewPawnCombat->OnAmmoCounterChanged.AddDynamic(this, &ThisClass::OnAmmoCounterChanged);
		NewPawnCombat->OnRoundFired.AddDynamic(this, &ThisClass::OnRoundFired);
		NewPawnCombat->OnAimingStatusChanged.AddDynamic(this, &ThisClass::OnAimingStatusChanged);
	}
}

void UShooterReticle::OnWeaponFirstReplicated(AWeapon* Weapon)
{
	OnReticleChanged(Weapon->GetReticleDynamic(), Weapon->ReticleParams);
	OnAmmoCounterChanged(Weapon->GetAmmoCounterDynamic(), Weapon->Ammo, Weapon->MagCapacity);
}

void UShooterReticle::OnReticleChanged(UMaterialInstanceDynamic* ReticleDynamic, const FReticleParams& ReticleParams)
{
	CurrentReticleParams = ReticleParams;
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

void UShooterReticle::OnRoundFired(int RoundsCurrent, int RoundsMax, int RoundsInReserve)
{
	_BaseCornerScaleFactor_RoundFired += CurrentReticleParams.ScaleFactor_RoundFired;
	_BaseShapeCutFactor_RoundFired += CurrentReticleParams.ShapeCutFactor_RoundFired;
	if (CurrentAmmoCounter.IsValid())
	{
		CurrentAmmoCounter->SetScalarParameterValue(Ammo::Rounds_Current, RoundsCurrent);
		CurrentAmmoCounter->SetScalarParameterValue(Ammo::Rounds_Max, RoundsMax);
	}
}

void UShooterReticle::OnAimingStatusChanged(bool bIsAiming)
{
	bAiming = bIsAiming;
}
