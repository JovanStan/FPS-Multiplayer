
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

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category="FPS|WeaponType")
	FGameplayTag WeaponType;

private:
	// Weapon Mesh: 1st person view
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;
	
	// Weapon Mesh: 3rd person view
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> ThirdPersonMesh;
	
public:
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	FORCEINLINE USkeletalMeshComponent* GetThirdPersonMesh() const { return ThirdPersonMesh; }
};
