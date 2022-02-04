// Fill out your copyright notice in the Description page of Project Settings.


#include "ChangeSpeed.h"
#include "AICharacter.h"
#include "AICharacterController.h"
#include "GameFramework/CharacterMovementComponent.h"


UChangeSpeed::UChangeSpeed()
{
	// sets a flag when the service is executed
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Change Speed");

}

void UChangeSpeed::OnBecomeRelevant(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	Super::OnBecomeRelevant(owner_comp, node_memory);
	auto const cont = owner_comp.GetAIOwner();
	AAICharacter* const npc = Cast<AAICharacter>(cont->GetPawn());
	npc->GetCharacterMovement()->MaxWalkSpeed = speed;

}


FString UChangeSpeed::GetStaticServiceDescription() const
{
	return FString("Change the NPC(AI)'s speed");
}
