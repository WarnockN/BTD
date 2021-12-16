// Fill out your copyright notice in the Description page of Project Settings.


#include "HordeGameMode.h"

#include "HealthComponent.h"
#include "TimerManager.h"
#include "EngineUtils.h"

AHordeGameMode::AHordeGameMode()
{
	TimeBetweenWaves = 2.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void AHordeGameMode::StartPlay()
{
	Super::StartPlay();
	
	PrepareForNextWave();
}

void AHordeGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
}

void AHordeGameMode::StartWave()
{
	WaveCount++;
	
	NumBotsToSpawn = 2 * WaveCount;
	
	GetWorldTimerManager().SetTimer(BotSpawnTimerHandle, this, &AHordeGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
}

void AHordeGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NumBotsToSpawn--;
	if (NumBotsToSpawn <= 0)
	{
		EndWave();
	}
}

void AHordeGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(BotSpawnTimerHandle);
}

void AHordeGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(NextWaveTimerHandle, this , &AHordeGameMode::StartWave, TimeBetweenWaves,false);
}

void AHordeGameMode::CheckWaveState()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(NextWaveTimerHandle);

	if (NumBotsToSpawn > 0 || bIsPreparingForWave)
	{
		return;
	}
	
	bool bIsAnyBotAlive = false;
	
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* TestPawn = *It;

		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		UHealthComponent* HealthComp =  Cast<UHealthComponent>(TestPawn->GetComponentByClass(UHealthComponent::StaticClass()));

		if (HealthComp && HealthComp->GetHealth() > 0.0f)
		{
			bIsAnyBotAlive = true;
			break;
		}
	}

	if (!bIsAnyBotAlive)
	{
		PrepareForNextWave();
	}
	
}


