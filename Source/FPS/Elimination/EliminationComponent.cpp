
#include "EliminationComponent.h"

#include "../Game/ShooterGameStateBase.h"
#include "../Player/ShooterPlayerState.h"
#include "../ShooterTypes/ShooterTypes.h"
#include "Kismet/GameplayStatics.h"

UEliminationComponent::UEliminationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SequentialEliminationInterval = 2.f;
	LastEliminationTime = 0.f;
	SequentialElimination = 0;
	Streak = 0;
	EliminationsNeededForStreak = 3;
}

void UEliminationComponent::OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal)
{
	AShooterPlayerState* AttackerPlayerState = GetPlayerStateFromActor(Attacker);
	if (!IsValid(AttackerPlayerState)) return;
	
	ProcessHitOrMiss(bHit, AttackerPlayerState);
	
	if (!bHit) return;
	
	AShooterPlayerState* VictimPlayerState = GetPlayerStateFromActor(Victim);
	if (!IsValid(VictimPlayerState)) return;
	
	if (bLethal)
	{
		ProcessElimination(bHeadShot, AttackerPlayerState, VictimPlayerState);
	}
	
}

AShooterPlayerState* UEliminationComponent::GetPlayerStateFromActor(AActor* Actor)
{
	APawn* Pawn = Cast<APawn>(Actor);
	if (IsValid(Pawn))
	{
		return Pawn->GetPlayerState<AShooterPlayerState>();
	}
	return nullptr;
}

void UEliminationComponent::ProcessHitOrMiss(bool bHit, AShooterPlayerState* AttackerPlayerState)
{
	if (bHit)
	{
		AttackerPlayerState->AddHit();
	}
	else
	{
		AttackerPlayerState->AddMiss();
	}
}

void UEliminationComponent::ProcessElimination(bool bHeadshot, AShooterPlayerState* AttackerPlayerState, AShooterPlayerState* VictimPlayerState)
{
	AttackerPlayerState->AddScoredElimination();
	VictimPlayerState->AddDefeat();
	
	ESpecialEliminationType SpecialEliminationType{};
	AShooterGameStateBase* ShooterGameState = Cast<AShooterGameStateBase>(UGameplayStatics::GetGameState(AttackerPlayerState));
	
	ProcessHeadshot(bHeadshot, SpecialEliminationType, AttackerPlayerState);
	ProcessSequentialElimination(SpecialEliminationType, AttackerPlayerState);
	ProcessStreaks(SpecialEliminationType, AttackerPlayerState, VictimPlayerState);
	if (IsValid(ShooterGameState))
	{
		ProcessFirstBlood(ShooterGameState, SpecialEliminationType, AttackerPlayerState);
	}
}

void UEliminationComponent::ProcessHeadshot(bool bHeadshot, ESpecialEliminationType& SpecialEliminationType, AShooterPlayerState* AttackerPlayerState)
{
	if (bHeadshot)
	{
		SpecialEliminationType |= ESpecialEliminationType::Headshot;
		AttackerPlayerState->AddHeadShotElimination();
	}
}

void UEliminationComponent::ProcessSequentialElimination(ESpecialEliminationType& SpecialEliminationType, AShooterPlayerState* AttackerPlayerState)
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastEliminationTime <= SequentialEliminationInterval)
	{
		++SequentialElimination;
	}
	else
	{
		SequentialElimination = 1;
	}
	LastEliminationTime = CurrentTime;
	
	if (SequentialElimination > 1)
	{
		SpecialEliminationType |= ESpecialEliminationType::Sequential;
		AttackerPlayerState->AddSequentialEliminations(SequentialElimination);
	}
}

void UEliminationComponent::ProcessStreaks(ESpecialEliminationType& SpecialEliminationType, AShooterPlayerState* AttackerPlayerState, AShooterPlayerState* VictimPlayerState)
{
	++Streak;
	if (Streak >= EliminationsNeededForStreak)
	{
		SpecialEliminationType |= ESpecialEliminationType::Streak;
		AttackerPlayerState->SetOnStreak(true);
		AttackerPlayerState->UpdateHighestStreak(Streak);
	}
	
	if (VictimPlayerState->IsOnStreak())
	{
		SpecialEliminationType |= ESpecialEliminationType::Showstopper;
		VictimPlayerState->AddShowStopperElimination();
		VictimPlayerState->SetOnStreak(false);
	}
	
	if (AttackerPlayerState->GetLastAttacker() == VictimPlayerState)
	{
		SpecialEliminationType |= ESpecialEliminationType::Revenge;
		AttackerPlayerState->AddRevengeElimination();
		AttackerPlayerState->SetLastAttacker(nullptr);
	}
	
	VictimPlayerState->SetLastAttacker(AttackerPlayerState);
}

void UEliminationComponent::ProcessFirstBlood(AShooterGameStateBase* ShooterGameState, ESpecialEliminationType& SpecialEliminationType, AShooterPlayerState* AttackerPlayerState)
{
	if (!ShooterGameState->HasFirstBloodBeenHad())
	{
		SpecialEliminationType |= ESpecialEliminationType::FirstBlood;
		AttackerPlayerState->GotFirstBlood();
	}
}


