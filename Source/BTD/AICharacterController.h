// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AICharacterController.generated.h"

/**
 * 
 */
UCLASS()
class BTD_API AAICharacterController : public AAIController
{
	GENERATED_BODY()

public:
	AAICharacterController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float deltaSeconds) override;
	virtual FRotator GetControlRotation() const override;

	UFUNCTION()
	void OnPawnDetected(const TArray<AActor*> &detectedPawns);

	//How far the AI can see in a circle around it.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	float sightRadius = 500.0f;

	//How long an object is kept in memory after it is detected
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	float sightAge = 5.0f;

	//AI loses right after being 50 units out of sightRadius
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	float loseSightRadius = sightRadius + 50.0f;

	//FOV of the AI in degrees
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	float fov = 110.0f;

	//Forward declaration of Unreal AI sight component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	class UAISenseConfig_Sight* sightConfig;
	
	//distance of AI to player
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	float distanceToPlayer = 0.0f;

	//Has AI detected player boolean?
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsPlayerDetected = false;
};
