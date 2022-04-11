// Fill out your copyright notice in the Description page of Project Settings.


#include "TrackerBot.h"

#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "HealthComponent.h"
#include "NetCharacter.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"

static int32 DebugTrackerBotDrawing = 0;
FAutoConsoleVariableRef CVARDebugTrackerBotDrawing(
	TEXT("COOP.DebugTrackerBot"),
	DebugTrackerBotDrawing,
	TEXT("Draw Debug Lines for TrackerBot"),
	ECVF_Cheat);

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

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetSphereRadius(200);
	SphereComp->SetupAttachment(RootComponent);
	
	bUseVelocityChange = false;
	MovementForce = 1000;
	RequiredDistanceToTarget = 100;

	ExplosionRadius = 350.0f;
	ExplosionDamage = 60.0f;

	SelfDamageInterval = 0.25f;
}

// Called when the game starts or when spawned
void ATrackerBot::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		NextPathPoint = GetNextPathPoint();
	}
	
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

	if (Health <= 0.0f)
	{
		SelfDestruct(); 
	}
}

FVector ATrackerBot::GetNextPathPoint()
{
	//get player location hack
	ACharacter* NetCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	//return next point in path
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), NetCharacter);
	if (NavPath && NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}

	//failed to find path
	return GetActorLocation();
}

void ATrackerBot::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;
	
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	UGameplayStatics::PlaySound2D(GetWorld(), ExplodeSound);

	MeshComp->SetVisibility(false, true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	

	if (HasAuthority())
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
	
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.0f, 0, 1.0f);
		
		SetLifeSpan(2.0f);
	}
}

void ATrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

// Called every frame
void ATrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority() && !bExploded)
	{
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

			if (DebugTrackerBotDrawing)
			{
				DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDir, 32, FColor::Yellow, false, 0.0f, 1.0f);
			}
			
		}
		if (DebugTrackerBotDrawing)
		{
			DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);
		}
		
	}
}

void ATrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (!bStartedSelfDestruct && !bExploded)
	{
		ANetCharacter* NetCharacter = Cast<ANetCharacter>(OtherActor);

		if (NetCharacter && !UHealthComponent::IsFriendly(OtherActor, this))
		{
			// overlapped with a player

			if (HasAuthority())
			{
				//start self destruct
				GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &ATrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);
			}	
			
			bStartedSelfDestruct = true;

			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
		}
	}
	
}

