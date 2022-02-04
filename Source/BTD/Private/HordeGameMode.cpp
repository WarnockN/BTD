// Fill out your copyright notice in the Description page of Project Settings.


#include "HordeGameMode.h"

#include "HealthComponent.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "HordeGameState.h"
#include "GameFramework/GameMode.h"

AHordeGameMode::AHordeGameMode()
{
	TimeBetweenWaves = 2.0f;
	GameStateClass = AHordeGameState::StaticClass();

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
	CheckAnyPlayerAlive();

}

void AHordeGameMode::StartWave()
{
	WaveCount++;
	
	NumBotsToSpawn = 2 * WaveCount;
	
	GetWorldTimerManager().SetTimer(BotSpawnTimerHandle, this, &AHordeGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);

	SetWaveState(EWaveState::WaveInProgress);
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

	SetWaveState(EWaveState::WaitingToComplete);
}

void AHordeGameMode::GameOver()
{
	EndWave();

	// @TODO: Finish up match, present game over to player
	UE_LOG(LogTemp, Log, TEXT("Game over, all palyers died!"));

	SetWaveState(EWaveState::GameOver);
}

void AHordeGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(NextWaveTimerHandle, this , &AHordeGameMode::StartWave, TimeBetweenWaves,false);

	SetWaveState(EWaveState::WaitingToStart);

	RestartDeadPlayers();
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

		SetWaveState(EWaveState::WaveComplete);
		PrepareForNextWave();
	}
	
}

void AHordeGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			UHealthComponent* HealthComp =  Cast<UHealthComponent>(MyPawn->GetComponentByClass(UHealthComponent::StaticClass()));

			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f) 
			{
				//player is still alive
				return;
			}
			//no player alive
			GameOver();
		}
	}
}

void AHordeGameMode::RestartDeadPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn() == nullptr)
		{
			RestartPlayer(PC);
		}
	}
}

void AHordeGameMode::SetWaveState(EWaveState NewState)
{
	AHordeGameState* GS = GetGameState<AHordeGameState>();

	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewState);
	}
}


