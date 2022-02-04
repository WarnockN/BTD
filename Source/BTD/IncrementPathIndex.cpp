// Fill out your copyright notice in the Description page of Project Settings.


#include "IncrementPathIndex.h"
#include "AICharacterController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AICharacter.h"
#include "blackboard_keys.h"
#include <cmath>


UIncrementPathIndex::UIncrementPathIndex(FObjectInitializer const& object_initializer)
{
    NodeName = TEXT("Increment Path Index");
}

EBTNodeResult::Type UIncrementPathIndex::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
    //get the AI controller
    AAICharacterController* const controller = Cast<AAICharacterController>(owner_comp.GetAIOwner());
    AAICharacter* const npc = Cast<AAICharacter>(controller->GetPawn());
    int const no_of_points = npc->get_patrol_path()->num();
    int const min_index = 0;
    int const max_index = no_of_points - 1;

    // get and set the blackboard index key
    int index = controller->get_blackboard()->GetValueAsInt(bb_keys::patrol_path_index);

    if (bidirectional)
    {
        if (index >= max_index && direction == EDirectionType::Forward)
        {
            direction = EDirectionType::Reverse;
        }
        else if (index == min_index && direction == EDirectionType::Reverse)
        {
            direction = EDirectionType::Forward;
        }
    }

    controller->get_blackboard()->SetValueAsInt(bb_keys::patrol_path_index,
        (direction == EDirectionType::Forward ? std::abs(++index) : std::abs(--index)) % no_of_points);

    // finish with success
    FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}
