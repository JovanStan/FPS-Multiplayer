
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

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
	void FireEffects(const FVector& ImpactPoint, const FVector& ImpactNormal, TEnumAsByte<EPhysicalSurface> SurfaceType, bool bIsFirstPerson);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS|WeaponType")
	FGameplayTag WeaponType;
	UPROPERTY(EditAnywhere, Category="FPS|FireType")
	TEnumAsByte<EFireType> FireType;
	
	UPROPERTY(EditAnywhere)
	float FireTime = .1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS|WeaponType")
	float AimFieldOfView;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void FireEffectsEvent(const FVector& ImpactPoint, const FVector& ImpactNormal, EPhysicalSurface ImpactSurfaceType, bool bIsFirstPerson);

private:
	// Weapon Mesh: 1st person view
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;
	
	// Weapon Mesh: 3rd person view
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> ThirdPersonMesh;
	
	void SetMeshVisibilities(const APawn* OwningPawn) const;
	
public:
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	FORCEINLINE USkeletalMeshComponent* GetThirdPersonMesh() const { return ThirdPersonMesh; }
};
