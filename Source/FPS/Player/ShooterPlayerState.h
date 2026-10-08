
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
	
	void AddScoredKill();
	void AddDefeat();
	void AddHit();
	void AddMiss();
	void AddHeadShotKill();
	void AddSequentialKills(int32 SequenceCount);
	void UpdateHighestStream(int32 StreakCount);
	void AddRevengeKill();
	void AddDethroneKill();
	void AddShowStopperKill();
	void GotFirstBlood();
	void IsWinner();
	
private:
	int32 ScoredKills;
	int32 Defeats;
	int32 Hits;
	int32 Misses;
	bool bOnStreak;
	int32 HeadShotsKills;
	
	// multiple kills within a short period of time
	TMap<int32, int32> SequentialKills;
	int32 HighestStreak;
	int32 RevengeKills;
	int32 DethroneKills;
	int32 ShowStopperKills;
	bool bFirstBlood;
	bool bWinner;
	
	
};
