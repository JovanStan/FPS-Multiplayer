
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ShooterGameStateBase.generated.h"


class AShooterPlayerState;

UCLASS()
class FPS_API AShooterGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AShooterGameStateBase();
	
	bool HasFirstBloodBeenHad() const { return bHasFirstBloodBeenHad; }
	void UpdateLeader();
	AShooterPlayerState* GetSoleLeader() const;
	bool IsTiedForTheLead(AShooterPlayerState* PlayerState) const;
	
private:
	bool bHasFirstBloodBeenHad;
	
	UPROPERTY()
	TArray<TObjectPtr<AShooterPlayerState>> Leaders;
};
