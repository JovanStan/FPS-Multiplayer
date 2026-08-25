
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;
class UWeaponData;
class UMaterialInstanceDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReticleChanged, UMaterialInstanceDynamic*, ReticleDynamic, const FReticleParams&, ReticleParams);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAmmoCounterChanged, UMaterialInstanceDynamic*, AmmoDynamic, int32, RoundsCurrent, int32, RoundMax);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRoundFired, int, RoundsCurrent, int, RoundsMax, int, RoundsInReserve);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimingStatusChanged, bool, bIsAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCurrentReserveAmmoChanged, int32, RoundsInReserve, int32, RoundsInWeapon);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UCombatComponent* FindCombatComponent(const AActor* Actor) { return IsValid(Actor) ? Actor->FindComponentByClass<UCombatComponent>() : nullptr;}
	
	UCombatComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void Initiate_CycleWeapon();
	void Initiate_FireWeapon_Pressed();
	void Initiate_FireWeapon_Released();
	void Initiate_ReloadWeapon();
	void Initiate_Aim_Pressed();
	void Initiate_Aim_Released();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FPS|Weapon")
	TObjectPtr<UWeaponData> WeaponData;
	
	void Equip(AWeapon* Weapon);
	void SpawnInventory();
	void DestroyInventory();
	
	void InitializeWeaponWidgets();
	
	UPROPERTY(BlueprintAssignable)
	FReticleChanged OnReticleChanged;
	UPROPERTY(BlueprintAssignable)
	FAmmoCounterChanged OnAmmoCounterChanged;
	UPROPERTY(BlueprintAssignable)
	FRoundFired OnRoundFired;
	UPROPERTY(BlueprintAssignable)
	FAimingStatusChanged OnAimingStatusChanged;
	UPROPERTY(BlueprintAssignable)
	FCurrentReserveAmmoChanged OnCurrentReserveAmmoChanged;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bAiming = false;
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentReserveAmmo)
	int32 CurrentReserveAmmo;
	UFUNCTION()
	void OnRep_CurrentReserveAmmo();

	
protected:
	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing=OnRep_CurrentWeapon)
	TObjectPtr<AWeapon> CurrentWeapon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TraceDistance = 20000.f;
	
private:
	TMap<FGameplayTag, int32> ReserveAmmo;
	
	UFUNCTION()
	void OnRep_CurrentWeapon(AWeapon* LastWeapon);
	
	UPROPERTY(Transient,  Replicated)
	TArray<AWeapon*> Inventory;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AWeapon>> DefaultWeaponClasses;
	
	AWeapon* SpawnWeapon(TSubclassOf<AWeapon> WeaponClass);
	
	UFUNCTION(Server, Reliable)
	void Server_Aim(bool bPressed);
	void Local_Aim(bool bPressed);
	
	void Local_FireWeapon();
	UFUNCTION(Server, Reliable)
	void Server_FireWeapon(const FHitResult& HitResult);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FireWeapon(const FHitResult& HitResult, int AuthAmmo);
	
	bool bTriggerPressed = false;
	FTimerHandle FireTimer;
	void FireTimerFinished();
	
public:
	FORCEINLINE AWeapon* GetCurrentWeapon() { return CurrentWeapon;}
};
