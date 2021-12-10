// Fill out your copyright notice in the Description page of Project Settings.


#include "FindPatrolPathPoint.h"
#include "AICharacterController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AICharacter.h"
#include "blackboard_keys.h"

UFindPatrolPathPoint::UFindPatrolPathPoint(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Find Patrol Path Point");
}

EBTNodeResult::Type UFindPatrolPathPoint::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{

	//get the AI Controller for patrolling NPC(AI)
	AAICharacterController* const controller = Cast<AAICharacterController>(owner_comp.GetAIOwner());

	//get current control path index from blackboard
	int const index = controller->get_blackboard()->GetValueAsInt(bb_keys::patrol_path_index);

	//use the index to get current patrol path from the NPC(AI)'s reference to the patrol path
	AAICharacter* const npc = Cast<AAICharacter>(controller->GetPawn());
	FVector const point = npc->get_patrol_path()->get_patrol_point(index);

	// transform this point to a global position using its parent
	FVector const global_point = npc->get_patrol_path()->GetActorTransform().TransformPosition(point);

	// write the current global path point to blackboard
	controller->get_blackboard()->SetValueAsVector(bb_keys::patrol_path_vector, global_point);

	// finish with success
	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
