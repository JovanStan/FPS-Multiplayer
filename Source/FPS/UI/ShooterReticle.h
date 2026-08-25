
#pragma once

#include "CoreMinimal.h"
#include "FPS/ShooterTypes/ShooterTypes.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "ShooterReticle.generated.h"


class AWeapon;
class UImage;

UCLASS()
class FPS_API UShooterReticle : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ReticleImage;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> AmmoCounterImage;
	
private:
	TWeakObjectPtr<UMaterialInstanceDynamic> CurrentReticle;
	TWeakObjectPtr<UMaterialInstanceDynamic> CurrentAmmoCounter;
	FReticleParams CurrentReticleParams;
	
	float BaseCornerScaleFactor;
	float BaseShapeCutFactor;
	float _BaseCornerScaleFactor_RoundFired;
	float _BaseShapeCutFactor_RoundFired;
	float _BaseCornerScaleFactor_Aiming;
	float _BaseShapeCutFactor_Aiming;
	bool bAiming;
	
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UFUNCTION()
	void OnWeaponFirstReplicated(AWeapon* Weapon);
	
	UFUNCTION()
	void OnReticleChanged(UMaterialInstanceDynamic* ReticleDynamic, const FReticleParams& ReticleParams);
	UFUNCTION()
	void OnAmmoCounterChanged(UMaterialInstanceDynamic* AmmoCounterDynamic, int RoundsCurrent, int RoundsMax);
	UFUNCTION()
	void OnRoundFired(int RoundsCurrent, int RoundsMax, int RoundsInReserve);
	UFUNCTION()
	void OnAimingStatusChanged(bool bIsAiming);
};

