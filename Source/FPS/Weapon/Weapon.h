
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FPS/ShooterTypes/ShooterTypes.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"
UENUM(BlueprintType)
enum class EWeaponStatus : uint8
{
	Idle,
	Firing,
	Reloading,
	Cycling,
	Unequipped
};

UENUM(BlueprintType)
enum EFireType : uint8
{
	Automatic,
	SemiAutomatic
};

UCLASS()
class FPS_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void OnRep_Instigator() override;
	
	void AttachToOwningPawn() const;
	void WeaponTrace(FHitResult& HitResult, float TraceDistance);
	void DryFire();
	void Local_Fire(const FVector& ImpactPoint, const FVector& ImpactNormal, TEnumAsByte<EPhysicalSurface> SurfaceType, bool bIsFirstPerson);
	void Auth_Fire();
	void Rep_Fire(int AuthAmmo);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS|WeaponType")
	FGameplayTag WeaponType;
	UPROPERTY(EditAnywhere, Category="FPS|FireType")
	TEnumAsByte<EFireType> FireType;
	
	EWeaponStatus WeaponStatus;
	
	UPROPERTY(EditAnywhere)
	float FireTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS|WeaponType")
	float AimFieldOfView;
	
	UPROPERTY(EditDefaultsOnly)
	FReticleParams ReticleParams;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> WeaponIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Ammo")
	int Ammo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Ammo")
	int MagCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Ammo")
	int StartingCarriedAmmo;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void FireEffectsEvent(const FVector& ImpactPoint, const FVector& ImpactNormal, EPhysicalSurface ImpactSurfaceType, bool bIsFirstPerson);
	UFUNCTION(BlueprintImplementableEvent)
	void DryFireEvent();
private:
	// Weapon Mesh: 1st person view
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;
	// Weapon Mesh: 3rd person view
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> ThirdPersonMesh;
	
	void SetMeshVisibilities(const APawn* OwningPawn) const;
	
	int Sequence;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> ReticleMaterial;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> AmmoCounterMaterial;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ReticleDynamic;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> AmmoCounterDynamic;
	
public:
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	FORCEINLINE USkeletalMeshComponent* GetThirdPersonMesh() const { return ThirdPersonMesh; }
	UMaterialInstanceDynamic* GetReticleDynamic(); 
	UMaterialInstanceDynamic* GetAmmoCounterDynamic();
};
