
#include "ShooterGameStateBase.h"

AShooterGameStateBase::AShooterGameStateBase()
{
	bHasFirstBloodBeenHad = false;
}

void AShooterGameStateBase::UpdateLeader()
{
	bHasFirstBloodBeenHad = true;
}
