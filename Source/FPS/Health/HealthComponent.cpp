
#include "HealthComponent.h"

#include "Net/UnrealNetwork.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	DeathState = EDeathState::NotDead;
	SetIsReplicatedByDefault(true);
	
	Heath = 100.f;
	MaxHealth = 100.f;
}

float UHealthComponent::GetHealthNormalized() const
{
	return (MaxHealth > 0.f) ? (Heath / MaxHealth) : 0.f;
}

bool UHealthComponent::ChangeHealthByAmount(float Amount, AActor* Instigator)
{
	float OldValue = Heath;
	Heath = FMath::Clamp(Heath + Amount, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(this, OldValue, Heath, Instigator);
	
	if (Heath <= 0.f)
	{
		StartDeath();
	}
	
	return false;
}

void UHealthComponent::StartDeath()
{
	if (DeathState != EDeathState::NotDead)
	{
		return;
	}
	
	DeathState = EDeathState::DeathStarted;
	OnDeathStarted.Broadcast();
	GetOwner()->ForceNetUpdate();
}

void UHealthComponent::ChangeMaxHealthByAmount(float Amount, AActor* Instigator)
{
	float OldValue = MaxHealth;
	MaxHealth += Amount;
	OnMaxHealthChanged.Broadcast(this, OldValue, MaxHealth, Instigator);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, DeathState);
	DOREPLIFETIME_CONDITION(ThisClass, Heath, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, MaxHealth, COND_OwnerOnly);
}

void UHealthComponent::OnRep_DeathState(EDeathState OldDeathState)
{
	if (DeathState == EDeathState::DeathStarted)
	{
		OnDeathStarted.Broadcast();
	}
}

void UHealthComponent::OnRep_Health(float OldValue)
{
	OnHealthChanged.Broadcast(this, OldValue, Heath, nullptr);
}

void UHealthComponent::OnRep_MaxHealth(float OldValue)
{
	OnMaxHealthChanged.Broadcast(this, OldValue, MaxHealth, nullptr);
}


