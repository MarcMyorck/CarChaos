// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MainHUDWidget.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "CarChaosCarPawnSphere.generated.h"

UCLASS()
class CARCHAOS_API ACarChaosCarPawnSphere : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACarChaosCarPawnSphere();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
	float MaxGas = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
	float CurrentGas = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
	float GasUsage = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
	float GasPickupValue = 30.f;

	void UpdateGasBarValue();

	void AddGas();

	UPROPERTY(BlueprintReadOnly, Category = "Car")
	USphereComponent* CarCollision;

	UPROPERTY(BlueprintReadOnly, Category = "Car")
	UArrowComponent* ArrowForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AccelerationPower = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BrakingPower = -4000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Grip = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SteeringStrength = 10000.f;

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
