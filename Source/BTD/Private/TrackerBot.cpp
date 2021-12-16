// Fill out your copyright notice in the Description page of Project Settings.


#include "TrackerBot.h"

#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "HealthComponent.h"
#include "GameFramework/Character.h"

// Sets default values
ATrackerBot::ATrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ATrackerBot::HandleTakeDamage);
	
	bUseVelocityChange = false;
	MovementForce = 1000;

	RequiredDistanceToTarget = 100;
}

// Called when the game starts or when spawned
void ATrackerBot::BeginPlay()
{
	Super::BeginPlay();

	FVector NextPoint = GetNextPathPoint();
	
}

void ATrackerBot::HandleTakeDamage(UHealthComponent* OwningHealthComp, float Health,
									float HealthDelta, const UDamageType* DamageType,
									AController* InstigatedBy, AActor* DamageCauser)
{
	//Explode on death

	// @TODO: Pulse the material on hit

	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
	
	

	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());
}

FVector ATrackerBot::GetNextPathPoint()
{
	//get player location hack
	ACharacter* NetCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	//return next pointin path
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), NetCharacter);
	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}

	//failed to find path
	return GetActorLocation();
}

// Called every frame
void ATrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();
	
	if (DistanceToTarget <= RequiredDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();

		DrawDebugString(GetWorld(), GetActorLocation(), "Target reached!");
		
	}
	else
	{
		//keep moving towards next target
		FVector ForceDir = NextPathPoint - GetActorLocation();
		ForceDir.Normalize();

		ForceDir *= MovementForce;
		
		MeshComp->AddForce(ForceDir, NAME_None, bUseVelocityChange);
		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDir, 32, FColor::Yellow, false, 0.0f, 1.0f);
	}
	DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);
}

