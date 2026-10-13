
#include "ShooterPlayerState.h"

AShooterPlayerState::AShooterPlayerState()
{
	SetNetUpdateFrequency(100.f);
	 ScoredEliminations = 0;
	 Defeats = 0;
	 Hits = 0;
	 Misses = 0;
	 bOnStreak = false;
	 HeadShotsEliminations = 0;

	 HighestStreak = 0;
	 RevengeEliminations = 0;
	 DethroneEliminations = 0;
	 ShowStopperEliminations = 0;
	 bFirstBlood = false;
	 bWinner = false;
}

void AShooterPlayerState::AddScoredElimination()
{
	++ScoredEliminations;
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

void AShooterPlayerState::AddHeadShotElimination()
{
	++HeadShotsEliminations;
}

void AShooterPlayerState::AddSequentialEliminations(int32 SequenceCount)
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

void AShooterPlayerState::UpdateHighestStreak(int32 StreakCount)
{
	if (StreakCount > HighestStreak)
	{
		HighestStreak = StreakCount;
	}
}

void AShooterPlayerState::AddRevengeElimination()
{
	++RevengeEliminations;
}

void AShooterPlayerState::AddDethroneElimination()
{
	++DethroneEliminations;
}

void AShooterPlayerState::AddShowStopperElimination()
{
	++ShowStopperEliminations;
}

void AShooterPlayerState::GotFirstBlood()
{
	bFirstBlood = true;
}

void AShooterPlayerState::IsWinner()
{
	bWinner = true;
}

void AShooterPlayerState::SetOnStreak(bool bIsOnStreak)
{
	bOnStreak = bIsOnStreak;
}

bool AShooterPlayerState::IsOnStreak() const
{
	return bOnStreak;
}

void AShooterPlayerState::SetLastAttacker(APlayerState* AttackerPlayerState)
{
	LastAttacker = AttackerPlayerState;
}

APlayerState* AShooterPlayerState::GetLastAttacker() const
{
	return LastAttacker.IsValid() ? LastAttacker.Get() : nullptr;
}

void AShooterPlayerState::Client_ScoredElimination_Implementation(int32 EliminationScore)
{
	
}

void AShooterPlayerState::Client_SpecialElimination_Implementation( const ESpecialEliminationType& SpecialEliminationType, int32 SequentialEliminationCount, int32 StreakCount, int32 EliminationScore)
{
	
}

void AShooterPlayerState::Client_LostTheLead_Implementation()
{
	
}
