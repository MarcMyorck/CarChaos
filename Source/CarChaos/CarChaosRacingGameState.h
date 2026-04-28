// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Sound/SoundBase.h"
#include "GameFramework/GameStateBase.h"
#include "CarChaosRacingGameState.generated.h"

class ACarChaosCarPawnPC;
class ARacingCheckpoint;
class ACarChaosPlayerController;
class UMainHUDWidget;

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

	void SetupStartingSequence();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Starting Sequence")
	USoundBase* StartingSound1;

	void PlayStartingSound1();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Starting Sequence")
	USoundBase* StartingSound2;

	void PlayStartingSound2();

	void FinishRace(bool IsWin);

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
