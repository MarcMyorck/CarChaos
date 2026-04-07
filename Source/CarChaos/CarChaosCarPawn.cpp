// Fill out your copyright notice in the Description page of Project Settings.


#include "CarChaosCarPawn.h"

// Sets default values
ACarChaosCarPawn::ACarChaosCarPawn()
{

}

// Called when the game starts or when spawned
void ACarChaosCarPawn::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;

    CarCollision = FindComponentByClass<UBoxComponent>();
}

// Called every frame
void ACarChaosCarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACarChaosCarPawn::UpdateGasBarValue()
{
    CurrentGas = FMath::Clamp(CurrentGas - (GasUsage * (GetWorld()->GetDeltaSeconds())), 0.f, MaxGas);
}

void ACarChaosCarPawn::AddGas()
{
	CurrentGas = FMath::Clamp(CurrentGas + GasPickupValue, 0.f, MaxGas);
}

void ACarChaosCarPawn::ChangeSpeed(float SpeedValue)
{
    float DeltaTime = GetWorld()->GetDeltaSeconds();

    if (SpeedValue > 0)
    {
        CurrentSpeed = FMath::Clamp(CurrentSpeed + (AccelerationPower * DeltaTime), 0.f, TopSpeed);
    }
    else if (SpeedValue < 0)
    {
        CurrentSpeed = FMath::Clamp(CurrentSpeed + (BrakingPower * DeltaTime), -TopSpeed, 0.f);
    }
    else
    {
        CurrentSpeed = FMath::FInterpTo(CurrentSpeed, 0.f, DeltaTime, 2.0f);
    }

    FVector ForceToAdd = GetActorForwardVector() * CurrentSpeed * CarCollision->GetMass();

    CarCollision->AddForce(ForceToAdd);
}

void ACarChaosCarPawn::Steer(float SteeringValue)
{
    float DeltaTime = GetWorld()->GetDeltaSeconds();

    // Smooth steering input
    CurrentSteering = FMath::FInterpTo(CurrentSteering, SteeringValue, DeltaTime, 5.0f);

    // Reduce steering at high speed
    float SpeedFactor = FMath::Clamp(CurrentSpeed / TopSpeed, 0.2f, 1.0f);

    float TurnTorque = CurrentSteering * SteeringStrength * SpeedFactor * CarCollision->GetMass();

    FVector Torque(0.f, 0.f, TurnTorque);
    CarCollision->AddTorqueInDegrees(Torque);
}

