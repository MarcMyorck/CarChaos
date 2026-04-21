// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/GameStateBase.h"
#include "CarChaosRacingGameState.generated.h"

class ACarChaosCarPawnPC;
class ARacingCheckpoint;

/**
 * 
 */
UCLASS()
class CARCHAOS_API ACarChaosRacingGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ACarChaosRacingGameState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoints")
	TArray<ARacingCheckpoint*> Checkpoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoints")
	bool IsCheckpointsSorted = false;

	void SortCheckpoints();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cars")
	TArray<ACarChaosCarPawnPC*> Cars;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Splines")
	TArray<USplineComponent*> RacingSplines;

	void UpdateCarRanking();

	void FinishRace();

	void LoseRace();

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
