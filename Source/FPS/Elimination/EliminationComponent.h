
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EliminationComponent.generated.h"

enum class ESpecialEliminationType : uint16;
class AShooterPlayerState;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPS_API UEliminationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEliminationComponent();

	UFUNCTION()
	void OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal);
	
	UPROPERTY(EditDefaultsOnly)
	float SequentialEliminationInterval;
	
private:
	
	float LastEliminationTime;
	int32 SequentialElimination;
	
	AShooterPlayerState* GetPlayerStateFromActor(AActor* Actor);
	void ProcessHitOrMiss(bool bHit, AShooterPlayerState* AttackerPlayerState);
	void ProcessElimination(bool bHeadshot, AShooterPlayerState* AttackerPlayerState, AShooterPlayerState* VictimPlayerState);
	void ProcessHeadshot(bool bHeadshot, ESpecialEliminationType& SpecialEliminationType, AShooterPlayerState* AttackerPlayerState);
	void ProcessSequentialElimination(ESpecialEliminationType& SpecialEliminationType, AShooterPlayerState* AttackerPlayerState);
};
