// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MainHUDWidget.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "CarChaosCarPawnPC.generated.h"

class ACarChaosRacingGameState;

UCLASS()
class CARCHAOS_API ACarChaosCarPawnPC : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACarChaosCarPawnPC();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameState")
	ACarChaosRacingGameState* GameState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player/Enemy")
	bool IsPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	int RoundsDone = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	int CurrentCheckpoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	int MaxCheckpoints = 31;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	int CurrentPosition = 0;

	void UpdateCheckpoint(int CheckpointNumber);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
	float MaxGas = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
	float CurrentGas = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
	float GasUsage = 3.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
	float GasPickupValue = 25.f;

	void UpdateGasBarValue();

	void AddGas();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float TimeLimit = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float TimeRemaining = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Points")
	float CurrentPoints = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Points")
	float PointsPerSecond = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Points")
	float PointsPerPickup = 250.f;

	UPROPERTY(BlueprintReadOnly, Category = "Car")
	UStaticMeshComponent* CarBodyMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Car")
	UArrowComponent* DirectionArrow;

	UPROPERTY(BlueprintReadOnly, Category = "Car")
	TArray<UStaticMeshComponent*> WheelMeshs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AccelerationPower = 350000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BrakingPower = -250000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Grip = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SteeringStrength = 800000000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CurrentSteering = 0.f;

	bool IsGrounded();

	void ChangeSpeed(float SpeedValue);

	void Steer(float SteeringValue);

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
