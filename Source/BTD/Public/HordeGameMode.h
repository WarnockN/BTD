// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "HordeGameState.h"
#include "GameFramework/GameModeBase.h"
#include "HordeGameMode.generated.h"

/**
 * 
 */

enum class EWaveState : uint8;

UCLASS()
class BTD_API AHordeGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	///start spawn bots
	void StartWave();

	//stop spawn bots
	void EndWave();

	void GameOver();

	//set timer for next wave
	void PrepareForNextWave();

	void CheckWaveState();

	void CheckAnyPlayerAlive();

	void SetWaveState(EWaveState WaveState);
	
	FTimerHandle BotSpawnTimerHandle;
	FTimerHandle NextWaveTimerHandle;
	

	// num bots to spawn in curr wave
	int32 NumBotsToSpawn;

	int32 WaveCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

	

public:
	AHordeGameMode();
	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};
