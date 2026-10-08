
#include "ShooterPlayerState.h"

AShooterPlayerState::AShooterPlayerState()
{
	SetNetUpdateFrequency(100.f);
	 ScoredKills = 0;
	 Defeats = 0;
	 Hits = 0;
	 Misses = 0;
	 bOnStreak = false;
	 HeadShotsKills = 0;

	 HighestStreak = 0;
	 RevengeKills = 0;
	 DethroneKills = 0;
	 ShowStopperKills = 0;
	 bFirstBlood = false;
	 bWinner = false;
}

void AShooterPlayerState::AddScoredKill()
{
	++ScoredKills;
}

void AShooterPlayerState::AddDefeat()
{
	++Defeats;
}

void AShooterPlayerState::AddHit()
{
	++Hits;
}

void AShooterPlayerState::AddMiss()
{
	++Misses;
}

void AShooterPlayerState::AddHeadShotKill()
{
	++HeadShotsKills;
}

void AShooterPlayerState::AddSequentialKills(int32 SequenceCount)
{
	if (SequentialKills.Contains(SequenceCount))
	{
		SequentialKills[SequenceCount]++;
	}
	else
	{
		SequentialKills.Add(SequenceCount, 1);
	}
	
	// Reduce the count for all lower sequence counts
	// this is because a triple kill means a double was scored first.
	// We want to count this as just a triple
	for (auto& Kil : SequentialKills)
	{
		if (Kil.Key < SequenceCount && Kil.Value > 0)
		{
			Kil.Value--;
		}
	}
}

void AShooterPlayerState::UpdateHighestStream(int32 StreakCount)
{
	if (StreakCount > HighestStreak)
	{
		HighestStreak = StreakCount;
	}
}

void AShooterPlayerState::AddRevengeKill()
{
	++RevengeKills;
}

void AShooterPlayerState::AddDethroneKill()
{
	++DethroneKills;
}

void AShooterPlayerState::AddShowStopperKill()
{
	++ShowStopperKills;
}

void AShooterPlayerState::GotFirstBlood()
{
	bFirstBlood = true;
}

void AShooterPlayerState::IsWinner()
{
	bWinner = true;
}
