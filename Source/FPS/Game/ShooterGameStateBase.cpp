
#include "ShooterGameStateBase.h"
#include "../Player/ShooterPlayerState.h"

AShooterGameStateBase::AShooterGameStateBase()
{
	bHasFirstBloodBeenHad = false;
}

void AShooterGameStateBase::UpdateLeader()
{
	TArray<APlayerState*> LocalSortedPlayers = PlayerArray;
	LocalSortedPlayers.Sort([](const APlayerState& A, const APlayerState& B)
	{
		const AShooterPlayerState* PlayerA = Cast<AShooterPlayerState>(&A);
		const AShooterPlayerState* PlayerB = Cast<AShooterPlayerState>(&B);
		return PlayerA->GetScoredEliminations() > PlayerB->GetScoredEliminations();
	});
	
	Leaders.Empty();
	
	if (LocalSortedPlayers.Num() > 0)
	{
		int32 HighestScore = 0;
		for (auto* Player : LocalSortedPlayers)
		{
			AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(Player);
			if (IsValid(ShooterPlayerState))
			{
				int32 PlayerScore = ShooterPlayerState->GetScoredEliminations();
				if (Leaders.Num() == 0)
				{
					HighestScore = PlayerScore;
					Leaders.Add(ShooterPlayerState);
				}
				else if (PlayerScore == HighestScore)
				{
					Leaders.Add(ShooterPlayerState); // Add to leaders if scores are tied
				}
				else
				{
					break; // As it's sorted, no need to check further
				}
			}
		}
	}
	
	bHasFirstBloodBeenHad = true;
}

AShooterPlayerState* AShooterGameStateBase::GetSoleLeader() const
{
	if (Leaders.Num() == 1)
	{
		return Leaders[0];
	}
	return nullptr;
}

bool AShooterGameStateBase::IsTiedForTheLead(AShooterPlayerState* PlayerState) const
{
	return Leaders.Contains(PlayerState);
}
