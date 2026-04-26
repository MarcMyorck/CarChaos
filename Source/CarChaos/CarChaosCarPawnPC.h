// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MainHUDWidget.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SplineComponent.h"
#include "CarChaosCarPawnPC.generated.h"

class ACarChaosRacingGameState;
class ARacingCheckpoint;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool IsRaceFinished = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	int RoundsDone = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	int CurrentCheckpoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	int MaxCheckpoints = 31;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	int CurrentPosition = 0;

	void UpdateCheckpoint(int CheckpointNumber);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AINavigation")
	ARacingCheckpoint* NextCheckpointObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AINavigation")
	USplineComponent* RacingSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AINavigation")
	float CurrentSplineDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AINavigation")
	float LookAheadDistance = 2000.f;

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
	bool IsInputEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AccelerationPower = 300000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BrakingPower = -250000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Grip = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SteeringStrength = 1600000000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CurrentSteering = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float OffroadPenalty = 0.60f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CurrentOffroadValue = 0.f;

	bool IsGrounded();

	void ChangeSpeed(float SpeedValue);

	void Steer(float SteeringValue);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Bounce Off")
	UCapsuleComponent* CarCollisionCapsule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Bounce Off")
	float BounceOffStrength = 50000.f;

	UFUNCTION()
	void OnCarHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropOil")
	TSubclassOf<AActor> OilObstacleBlueprintClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropOil")
	float DropOilCost = 10.f;

	void DropOil();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OilSlow")
	bool IsOilSlowed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OilSlow")
	float OilSlowTimer = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OilSlow")
	float OilSlowDuration = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OilSlow")
	float OilSlowFactor = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OilSlow")
	UMaterialInterface* OriginalTireMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OilSlow")
	UMaterialInterface* OilTireMat;

	void StartOilSlow();

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
