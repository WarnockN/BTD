// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerupActor.h"

// Sets default values
APowerupActor::APowerupActor()
{
	PowerupInterval = 0.0f;
	TotalNumOfTicks = 0;

}


// Called when the game starts or when spawned
void APowerupActor::BeginPlay()
{
	Super::BeginPlay();
}

void APowerupActor::ActivatePowerup()
{
	if (PowerupInterval > 0)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &APowerupActor::OnTickPowerup, PowerupInterval, true, 0.0f);
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

		//Delete timer
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

