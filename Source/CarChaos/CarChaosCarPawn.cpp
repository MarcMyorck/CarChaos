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
        FVector AccelerationForce = CarCollision->GetForwardVector() * AccelerationPower;
        CarCollision->AddForce(AccelerationForce);
    }
    else if (SpeedValue < 0)
    {
        FVector BrakingForce = CarCollision->GetForwardVector() * BrakingPower;
        CarCollision->AddForce(BrakingForce);

        FVector CurrentVelocity = CarCollision->GetPhysicsLinearVelocity();
        if (CurrentVelocity.Size() < 5.f)
        {
            CarCollision->SetPhysicsLinearVelocity(FVector::ZeroVector);
        }
    }
    else
    {
    }
}

void ACarChaosCarPawn::Steer(float SteeringValue)
{
    float DeltaTime = GetWorld()->GetDeltaSeconds();

    // Smooth steering input
    CurrentSteering = FMath::FInterpTo(CurrentSteering, SteeringValue, DeltaTime, 5.0f);

    float TurnTorque = CurrentSteering * SteeringStrength;

    FVector Torque(0.f, 0.f, TurnTorque);
    CarCollision->AddTorqueInDegrees(Torque);
}

