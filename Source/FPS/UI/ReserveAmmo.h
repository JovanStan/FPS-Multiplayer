
#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "ReserveAmmo.generated.h"


class AWeapon;
class UImage;
class UTextBlock;

UCLASS()
class FPS_API UReserveAmmo : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Ammo;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> WeaponIconImage;
	
private:
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	UFUNCTION()
	void OnCurrentReserveAmmoChanged(int32 RoundsInReserve, int32 RoundsInWeapon);
	UFUNCTION()
	void OnRoundFired(int32 RoundCurrent, int32 RoundMax, int RoundsInReserve);
	UFUNCTION()
	void OnWeaponFirstReplicated(AWeapon* Weapon);
};
