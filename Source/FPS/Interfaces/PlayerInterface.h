
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"


class AWeapon;
struct FGameplayTag;

UINTERFACE()
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};


class FPS_API IPlayerInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FName GetWeaponAttachPoint(const FGameplayTag& WeaponType) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USkeletalMeshComponent* GetFirstPersonMesh() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USkeletalMeshComponent* GetThirdPersonMesh() const;
	UFUNCTION(BlueprintNativeEvent)
	bool IsWeaponEquipped(const AWeapon* Weapon) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Notify_CycleWeapon();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Notify_ReloadWeapon();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddAmmo(const FGameplayTag& WeaponType, int32 Amount);
	
	UFUNCTION(BlueprintNativeEvent)
	void WeaponReplicated();
	
	UFUNCTION(BlueprintNativeEvent)
	AWeapon* GetCurrentWeapon();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetReserveAmmo();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool DoDamage(float DamageAmount, AActor* DamageInstigator);
};
