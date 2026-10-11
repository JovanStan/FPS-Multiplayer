
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ShooterPlayerState.generated.h"


UCLASS()
class FPS_API AShooterPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AShooterPlayerState();
	
	void AddScoredElimination();
	void AddDefeat();
	void AddHit();
	void AddMiss();
	void AddHeadShotElimination();
	void AddSequentialEliminations(int32 SequenceCount);
	void UpdateHighestStreak(int32 StreakCount);
	void AddRevengeElimination();
	void AddDethroneElimination();
	void AddShowStopperElimination();
	void GotFirstBlood();
	void IsWinner();
	
	void SetOnStreak(bool bIsOnStreak);
	bool IsOnStreak() const;
	
	void SetLastAttacker(APlayerState* AttackerPlayerState);
	APlayerState* GetLastAttacker() const;
	
private:
	int32 ScoredEliminations;
	int32 Defeats;
	int32 Hits;
	int32 Misses;
	bool bOnStreak;
	int32 HeadShotsEliminations;
	
	// multiple kills within a short period of time
	TMap<int32, int32> SequentialKills;
	int32 HighestStreak;
	int32 RevengeEliminations;
	int32 DethroneEliminations;
	int32 ShowStopperEliminations;
	bool bFirstBlood;
	bool bWinner;
	
	TWeakObjectPtr<APlayerState> LastAttacker;
};
