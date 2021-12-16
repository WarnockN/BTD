// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "GameFramework/Pawn.h"
#include "TrackerBot.generated.h"

class UHealthComponent;

UCLASS()
class BTD_API ATrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UHealthComponent* HealthComp;

	UFUNCTION()
	void HandleTakeDamage(UHealthComponent* OwningHealthComp, float Health,
									float HealthDelta, const UDamageType* DamageType,
									AController* InstigatedBy, AActor* DamageCauser);

	FVector GetNextPathPoint();

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float RequiredDistanceToTarget;

	//dynamic mat to pulse on dmg
	UMaterialInstanceDynamic* MatInst;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
