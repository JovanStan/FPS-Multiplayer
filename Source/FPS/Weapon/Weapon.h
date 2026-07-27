
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class FPS_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void OnRep_Instigator() override;
	
	void AttachToOwningPawn() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS|WeaponType")
	FGameplayTag WeaponType;

protected:
	virtual void BeginPlay() override;

private:
	// Weapon Mesh: 1st person view
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;
	
	// Weapon Mesh: 3rd person view
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> ThirdPersonMesh;
	
	void SetMeshVisibilities(const APawn* OwningPawn) const;
	
public:
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	FORCEINLINE USkeletalMeshComponent* GetThirdPersonMesh() const { return ThirdPersonMesh; }
};
