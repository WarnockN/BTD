// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerupActor.h"

#include "Net/UnrealNetwork.h" 

// Sets default values
APowerupActor::APowerupActor()
{
	PowerupInterval = 0.0f;
	TotalNumOfTicks = 0;

	bIsPowerupActive = false;

	SetReplicates(true);
}


// Called when the game starts or when spawned
void APowerupActor::BeginPlay()
{
	Super::BeginPlay();
}

void APowerupActor::ActivatePowerup(AActor* ActiveFor)
{
	OnActivated(ActiveFor);

	bIsPowerupActive = true;
	OnRep_PowerupActive();
	
	if (PowerupInterval > 0)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &APowerupActor::OnTickPowerup, PowerupInterval, true);
	}
	else
	{
		OnTickPowerup();
	}
}

void APowerupActor::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerupTicked();
	
	if (TicksProcessed >= TotalNumOfTicks)
	{
		OnExpired();
		
		bIsPowerupActive = false;
		OnRep_PowerupActive();

		//Delete timer
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void APowerupActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerupActive);
}

void APowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APowerupActor, bIsPowerupActive);
}

