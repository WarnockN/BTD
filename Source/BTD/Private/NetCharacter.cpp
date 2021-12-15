// Fill out your copyright notice in the Description page of Project Settings.


#include "NetCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Weapon.h"

// Sets default values
ANetCharacter::ANetCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	ZoomFOV = 65.0f;
	ZoomInterpSpeed = 20.0f;

	WeaponSocketName = "WeaponSocket";

}

// Called when the game starts or when spawned
void ANetCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;

	//spawn default weapon
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	CurrWeapon = GetWorld()->SpawnActor<AWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, ActorSpawnParameters);

	if (CurrWeapon)
	{
		CurrWeapon->SetOwner(this);
		CurrWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
	}
}




// Called every frame
void ANetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	
	CameraComp->SetFieldOfView(NewFOV);

}

void ANetCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ANetCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ANetCharacter::BeginCrouch()
{
	Crouch();
}

void ANetCharacter::EndCrouch()
{
	UnCrouch();
}

void ANetCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ANetCharacter::EndZoom()
{
	bWantsToZoom = false;	
}

void ANetCharacter::StartFire()
{
	if (CurrWeapon)
	{
		CurrWeapon->StartFire();
	}
}

void ANetCharacter::EndFire()
{
	if (CurrWeapon)
	{
		CurrWeapon->EndFire();
	}
}

// Called to bind functionality to input
void ANetCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ANetCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANetCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ANetCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ANetCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ANetCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ANetCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this,&ACharacter::Jump);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ANetCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ANetCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ANetCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ANetCharacter::EndFire);
}

FVector ANetCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
		
	}

	return Super::GetPawnViewLocation();
}

