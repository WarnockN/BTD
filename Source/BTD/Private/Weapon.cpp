// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShakeBase.h"

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

}

void AWeapon::Fire()
{
	//Trace world from pawn eyes to crosshair location (center of screen)
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		//get eye location and rotation -- the viewport
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		//set the trace end location
		FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * 10000);

		//add optional line trace query params
		FCollisionQueryParams QueryParams;

		//ignore owning character and weapon mesh
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);

		//do complex traces, costs more but idc
		QueryParams.bTraceComplex = true;

		//Particle "Target" param
		FVector TracerEndPoint = TraceEnd;

		//get the hit result struct -- for use later
		FHitResult Hit;

		//do the line trace from eyelocation to traceend
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			//Blocking hit here, process damage

			AActor* HitActor = Hit.GetActor();

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TracerEndPoint = Hit.ImpactPoint;
		}

		if (DebugWeaponDraw > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Purple, false, 5.0f, 1.0f, 5.0f);
		}
		
		PlayFireEffects(TracerEndPoint);
	}
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

