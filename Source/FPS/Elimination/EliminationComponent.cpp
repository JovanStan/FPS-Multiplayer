
#include "EliminationComponent.h"
#include "../Player/ShooterPlayerState.h"
#include "../ShooterTypes/ShooterTypes.h"

UEliminationComponent::UEliminationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SequentialEliminationInterval = 2.f;
	LastEliminationTime = 0.f;
	SequentialElimination = 0;
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
	AttackerPlayerState->AddScoredKill();
	VictimPlayerState->AddDefeat();
	
	 ESpecialEliminationType SpecialEliminationType{};
	
	ProcessHeadshot(bHeadshot, SpecialEliminationType, AttackerPlayerState);
	ProcessSequentialElimination(SpecialEliminationType, AttackerPlayerState);
}

void UEliminationComponent::ProcessHeadshot(bool bHeadshot, ESpecialEliminationType& SpecialEliminationType, AShooterPlayerState* AttackerPlayerState)
{
	if (bHeadshot)
	{
		SpecialEliminationType |= ESpecialEliminationType::Headshot;
		AttackerPlayerState->AddHeadShotKill();
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
		AttackerPlayerState->AddSequentialKills(SequentialElimination);
	}
}


