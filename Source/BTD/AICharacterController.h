// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BTDCharacter.h"
#include "Perception/AIPerceptionTypes.h"
#include "AICharacterController.generated.h"

/**
 * 
 */
UCLASS()
class BTD_API AAICharacterController : public AAIController
{
	GENERATED_BODY()

public:
	AAICharacterController(FObjectInitializer const& object_initializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* const pawn) override;
	class UBlackboardComponent* get_blackboard() const;
	virtual void on_target_detected(AActor* actor, FAIStimulus const stimulus);

	virtual void Tick(float deltaSeconds) override;

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* behavior_tree_component;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* btree;

	class UBlackboardComponent* blackboard;

	class UAISenseConfig_Sight* sight_config;

	UFUNCTION()
	void on_updated(TArray<AActor*> const& updated_actors);

	void setup_perception_system();

};
