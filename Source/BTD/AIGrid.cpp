// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGrid.h"

// Sets default values
AAIGrid::AAIGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GridSizeWorld.X = 500;
	GridSizeWorld.Y = 500;
	TileSize = 50;
}

// Called when the game starts or when spawned
void AAIGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector AAIGrid::GridBottomLeft() {
	FVector vector;
	vector.X = GridLocation.X - (GridSizeWorld.X);
	vector.Y = GridLocation.Y - (GridSizeWorld.Y);
	return vector;
}

FVector2D AAIGrid::GridTileNumber()
{
	FVector2D tileNum;
	tileNum.X = round((GridSizeWorld.X / TileSize) - 1);
	tileNum.X = round((GridSizeWorld.X / TileSize) - 1);

	return tileNum;
}

void AAIGrid::DrawTile()
{
	
	int X = 0;
	int Y = 0;
	for (int i = 0; i++; (i < GridTileNumber().X)) {
		X = i;
		for (int j = 0; j++; (j < GridTileNumber().Y)) {
			Y = j;
			TilePosition = GridBottomLeft() + (TileSize * 2 * X + TileSize) + (TileSize * 2 * Y + TileSize);
			//draw debug plane
		}
	}
	
	

}
