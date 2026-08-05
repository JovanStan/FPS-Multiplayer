
#pragma once

#include "CoreMinimal.h"
#include "FPS/Interfaces/PlayerInterface.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

enum class ETurningInPlace : uint8;
class UInputAction;
class UCombatComponent;
class UCameraComponent;
class USpringArmComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponFirstReplicated, AWeapon*, Weapon);

UCLASS()
class FPS_API AShooterCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AShooterCharacter();
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaSeconds) override;
	
	/** Player Interface */
	virtual FName GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const override;
	virtual USkeletalMeshComponent* GetFirstPersonMesh_Implementation() const override;
	virtual USkeletalMeshComponent* GetThirdPersonMesh_Implementation() const override;
	virtual bool IsWeaponEquipped_Implementation(const AWeapon* Weapon) const override;
	virtual void WeaponReplicated_Implementation() override;
	/** ~Player Interface */
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnAim(bool bIsAiming);
	
	UFUNCTION(BlueprintCallable)
	FRotator GetFixedAimRotation() const;
	UFUNCTION(BlueprintCallable)
	bool HasCurrentWeapon() const;
	
	UPROPERTY(BlueprintReadOnly)
	FTransform FABRIK_SocketTransform;
	
	UPROPERTY(BlueprintAssignable)
	FWeaponFirstReplicated OnWeaponFirstReplicated;


private:
	// 1st person view (arms)
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> FirstPersonCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TObjectPtr<UCombatComponent> CombatComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float DefaultFieldOfView;
	
	void CalculateFabrikSocketTransform();
	void CalculateTurnInPlaceParameters(float DeltaTime);
	void TurnInPlace(float DeltaTime);
	
	FRotator StartingAimRotation;
	float InterpAO_Yaw;
	
	bool bWeaponFirstReplicated;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float AO_Yaw;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float MovementOffsetYaw;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	ETurningInPlace TurningStatus;
	
public:
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE bool HasWeaponFirstReplicated() const { return  bWeaponFirstReplicated; }
};
