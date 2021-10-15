// Fill out your copyright notice in the Description page of Project Settings.


#include "Waypoint.h"

#include "AICharacter.h"

// Sets default values
AWaypoint::AWaypoint() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(root);

	boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	boxComponent->SetupAttachment(GetRootComponent());
	boxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWaypoint::OnPlayerEnter);
	

}

// Called when the game starts or when spawned
void AWaypoint::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void AWaypoint::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void AWaypoint::OnPlayerEnter(UPrimitiveComponent* overlapComponent, AActor* otherActor, UPrimitiveComponent* otherComp,
							  int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult) {
	AAICharacter* character = nullptr;

	if (otherActor != nullptr) {
		character = Cast<AAICharacter>(otherActor);

		if (character != nullptr) {
			character->nextWaypoint = nextWaypoint;
		}
	}
}

