// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerState.h"

void ANetPlayerState::AddScore(float ScoreDelta)
{
	SetScore(GetScore() + ScoreDelta);
}
