
#pragma once

#include "CoreMinimal.h"
#include "FPS/Interfaces/PlayerInterface.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

class UInputAction;
class UCombatComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class FPS_API AShooterCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AShooterCharacter();
	virtual void PossessedBy(AController* NewController) override;
	
	/** Player Interface */
	virtual FName GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const override;
	/** ~Player Interface */

protected:
	virtual void BeginPlay() override;

private:
	// 1st person view (arms)
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FirstPersonCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TObjectPtr<UCombatComponent> CombatComponent;
	
public:
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
};
