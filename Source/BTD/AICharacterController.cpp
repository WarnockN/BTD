// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacterController.h"
#include "AICharacter.h"
#include "BTDCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "blackboard_keys.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "GameFramework/Character.h"
#include "ai_tags.h"


AAICharacterController::AAICharacterController(FObjectInitializer const& object_initializer)
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/Blueprints/Charcters/BT_AI.BT_AI'"));
	if (obj.Succeeded())
	{
		btree = obj.Object;
	}

	behavior_tree_component = object_initializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	blackboard = object_initializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));
	setup_perception_system();

}

void AAICharacterController::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(btree);
	behavior_tree_component->StartTree(*btree);
}

void AAICharacterController::OnPossess(APawn* const pawn)
{
	Super::OnPossess(pawn);
	if (blackboard)
	{
		blackboard->InitializeBlackboard(*btree->BlackboardAsset);
	}

}

UBlackboardComponent* AAICharacterController::get_blackboard() const
{
	return blackboard;
}

//Might have to check if i did this right - ABTDCharacter or AAICharacter
void AAICharacterController::on_target_detected(AActor* actor, FAIStimulus const stimulus)
{
	if (auto const ch = Cast<ABTDCharacter>(actor))
	{
		get_blackboard()->SetValueAsBool(bb_keys::can_see_player, stimulus.WasSuccessfullySensed());
	}
}

void AAICharacterController::Tick(float deltaSeconds)
{
}

void AAICharacterController::on_updated(TArray<AActor*> const& updated_actors)
{
	for (size_t x = 0; x < updated_actors.Num(); ++x)
	{
		FActorPerceptionBlueprintInfo info;

		// gets actors perception and puts it in info
		GetPerceptionComponent()->GetActorsPerception(updated_actors[x], info);

		for (size_t k = 0; k < info.LastSensedStimuli.Num(); ++k)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("AI Controller - FLOOP1 Worked"));

			FAIStimulus const stim = info.LastSensedStimuli[k];
			if (stim.Tag == tags::noise_tag)
			{
				get_blackboard()->SetValueAsBool(bb_keys::is_investigating, stim.WasSuccessfullySensed());
				get_blackboard()->SetValueAsVector(bb_keys::target_location, stim.StimulusLocation);
			}
			
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("AI Controller - FLOOP2 Worked"));
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("AI Controller Updated"));
}

void AAICharacterController::setup_perception_system()
{
	//create and initialise sight configuration object
	sight_config = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

	if (sight_config)
	{
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
		sight_config->SightRadius = 500.0f;
		sight_config->LoseSightRadius = sight_config->SightRadius + 25.0f;
		sight_config->PeripheralVisionAngleDegrees = 90.0f;
		sight_config->SetMaxAge(5.0f);
		sight_config->AutoSuccessRangeFromLastSeenLocation = 520.0f;
		sight_config->DetectionByAffiliation.bDetectEnemies =
			sight_config->DetectionByAffiliation.bDetectFriendlies =
			sight_config->DetectionByAffiliation.bDetectNeutrals = true;

		// add sight configuration component to perception component
		GetPerceptionComponent()->SetDominantSense(*sight_config->GetSenseImplementation());

		//test
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAICharacterController::on_target_detected);

		GetPerceptionComponent()->ConfigureSense(*sight_config);
	}


	// create and initialise hearing config object
	hearing_config = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing config"));
	if (hearing_config)
	{
		hearing_config->HearingRange = 3000.0f;


		hearing_config->DetectionByAffiliation.bDetectEnemies =
			hearing_config->DetectionByAffiliation.bDetectFriendlies =
			hearing_config->DetectionByAffiliation.bDetectNeutrals = true;

		// add sight configuration component to perception component
		GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AAICharacterController::on_updated);
		GetPerceptionComponent()->ConfigureSense(*hearing_config);
	}
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Perception system updated"));

}
