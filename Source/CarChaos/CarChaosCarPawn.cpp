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

    CarCollision = FindComponentByClass<UCapsuleComponent>();
}

// Called every frame
void ACarChaosCarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (IsGrounded()) 
    {
        //Sideways Grip
        FVector Velocity = CarCollision->GetPhysicsLinearVelocity();

        FVector Forward = CarCollision->GetForwardVector();
        FVector Right = CarCollision->GetRightVector();

        float ForwardSpeed = FVector::DotProduct(Velocity, Forward);
        float SidewaysSpeed = FVector::DotProduct(Velocity, Right);

        FVector ForwardVelocity = Forward * ForwardSpeed;
        FVector SidewaysVelocity = Right * SidewaysSpeed;

        FVector NewVelocity = ForwardVelocity + (SidewaysVelocity * Grip);

        CarCollision->SetPhysicsLinearVelocity(NewVelocity);

        //Downforce
        FVector NewForwardVelocity = CarCollision->GetForwardVector() * FVector::DotProduct(CarCollision->GetPhysicsLinearVelocity(), CarCollision->GetForwardVector());
        float Speed = ForwardVelocity.Size();

        FVector Downforce = -FVector::UpVector * Speed * 2.5f;

        CarCollision->AddForce(Downforce);

        //General Settings
        CarCollision->SetLinearDamping(0.6f);
        CarCollision->SetAngularDamping(8.f);
    }
    else
    {
        //Downforce
        FVector Velocity = CarCollision->GetPhysicsLinearVelocity();
        float Speed = Velocity.Size();

        FVector Downforce = -FVector::UpVector * Speed * 0.5f;

        CarCollision->AddForce(Downforce);

        //General Settings
        CarCollision->SetLinearDamping(0.f);
        CarCollision->SetAngularDamping(20.f);
    }
}

void ACarChaosCarPawn::UpdateGasBarValue()
{
    CurrentGas = FMath::Clamp(CurrentGas - (GasUsage * (GetWorld()->GetDeltaSeconds())), 0.f, MaxGas);
}

void ACarChaosCarPawn::AddGas()
{
	CurrentGas = FMath::Clamp(CurrentGas + GasPickupValue, 0.f, MaxGas);
}

bool ACarChaosCarPawn::IsGrounded()
{
    FVector Start = CarCollision->GetComponentLocation();
    FVector End = Start - FVector(0, 0, 45.f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    return GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,
        Params
    );
}

void ACarChaosCarPawn::ChangeSpeed(float SpeedValue)
{
    if (!IsGrounded()) return;

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

    if (!IsGrounded())
    {
        CurrentSteering *= 0.8f;
    }

    float TurnTorque = CurrentSteering * SteeringStrength;

    FVector Torque(0.f, 0.f, TurnTorque);
    CarCollision->AddTorqueInDegrees(Torque);
}

