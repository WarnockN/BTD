// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "DrawDebugHelpers.h"
#include "BTD/BTD.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Net/UnrealNetwork.h"

static int32 DebugWeaponDraw = 0;
FAutoConsoleVariableRef CVARDebugWeaponDraw (
	TEXT("Multi.DebugWeapons"),
	DebugWeaponDraw,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

// Sets default values
AWeapon::AWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	BaseDamage = 20.0f;

	RateOfFire = 600;

	BulletSpread = 2.0f;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
}

void AWeapon::Fire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}
	//Trace world from pawn eyes to crosshair location (center of screen)
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		//get eye location and rotation -- the viewport
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		//bullet spread
		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		//set the trace end location
		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		//add optional line trace query params
		FCollisionQueryParams QueryParams;

		//ignore owning character and weapon mesh
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);

		//do complex traces, costs more but idc
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		//Particle "Target" param
		FVector TracerEndPoint = TraceEnd;

		//get the hit result struct -- for use later
		FHitResult Hit;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		//do the line trace from eyelocation to traceend
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			//Blocking hit here, process damage
			AActor* HitActor = Hit.GetActor();

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESH_VULNERABLE)
			{
				ActualDamage *= 2.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;

			
		}

		if (DebugWeaponDraw > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Purple, false, 5.0f, 1.0f, 5.0f);
		}
		
		PlayFireEffects(TracerEndPoint);

		if (GetLocalRole() == ROLE_Authority)
		{
			HitscanTrace.TraceEnd = TracerEndPoint;
			HitscanTrace.SurfaceType = SurfaceType;
		}

		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void AWeapon::ServerFire_Implementation()
{
	Fire();	
}

bool AWeapon::ServerFire_Validate()
{
	return true;
}

void AWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	
	GetWorldTimerManager().SetTimer(FireTimer, this, &AWeapon::Fire, TimeBetweenShots, true, FirstDelay);
	
	Fire();
}

void AWeapon::EndFire()
{
	GetWorldTimerManager().ClearTimer(FireTimer);
}


void AWeapon::OnRep_HitscanTrace()
{
	//Play FX
	PlayFireEffects(HitscanTrace.TraceEnd);

	PlayImpactEffects(HitscanTrace.SurfaceType, HitscanTrace.TraceEnd);

}

void AWeapon::PlayFireEffects(FVector TraceEnd)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}
}

void AWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_FLESH_DEFAULT:
	case SURFACE_FLESH_VULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeapon, HitscanTrace, COND_SkipOwner);
}

