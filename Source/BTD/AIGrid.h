// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIGrid.generated.h"

UCLASS()
class BTD_API AAIGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIGrid();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
		FVector GridLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
		UScene *Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
		FVector2D GridSizeWorld;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
		FVector TilePosition;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
		float TileSize;

	UFUNCTION(BlueprintCallable, Category = "Properties")
		FVector GridBottomLeft();
	UFUNCTION(BlueprintCallable, Category = "Properties")
		FVector2D GridTileNumber();
	UFUNCTION(BlueprintCallable, Category = "Properties")
		void DrawTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

};
