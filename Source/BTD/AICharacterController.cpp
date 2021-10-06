// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacterController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


AAICharacterController::AAICharacterController() {
	//Allow AI actor to Tick()
	PrimaryActorTick.bCanEverTick = true;

	//Setup sight config and Perception component
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

	//Set the AI sight settings
	{
		sightConfig->SightRadius = sightRadius;
		sightConfig->LoseSightRadius = loseSightRadius;
		sightConfig->PeripheralVisionAngleDegrees = fov;
		sightConfig->SetMaxAge(sightAge);

		//Detect enemies, allies, and neutrals
		sightConfig->DetectionByAffiliation.bDetectEnemies = true;
		sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		sightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	}

	//Set the AI's dominant sense to sight, our AI will never hear the player or use any other senses, only works on visuals
	GetPerceptionComponent()->SetDominantSense(*sightConfig->GetSenseImplementation());

	//Create delegate to call when something is detected by the AI
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AAICharacterController::OnPawnDetected);

	//Save all the settings for the sight configuration.
	GetPerceptionComponent()->ConfigureSense(*sightConfig);

	
}

void AAICharacterController::BeginPlay() {
	Super::BeginPlay();

	if (GetPerceptionComponent() != nullptr) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("All systems set up!"));
	else GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("A problem has aoccured."));
}

void AAICharacterController::OnPossess(APawn* inPawn) {
	Super::OnPossess(inPawn);
}

void AAICharacterController::Tick(float deltaSeconds) {

}

FRotator AAICharacterController::GetControlRotation() const {
	/*If no pawn exists, set rotation to 0
	  Otherwise, set rotation to the ai characters yaw. This makes them face the direction they are walking*/
	if (GetPawn() == nullptr) return FRotator(0.0f, 0.0f, 0.0f);
	
	return FRotator(0.0f, GetPawn()->GetActorRotation().Yaw, 0.0f);
}

void AAICharacterController::OnPawnDetected(const TArray<AActor*> &detectedPawns) {
	
}
