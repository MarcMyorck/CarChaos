// Fill out your copyright notice in the Description page of Project Settings.


#include "CarChaosCarPawnSphere.h"

// Sets default values
ACarChaosCarPawnSphere::ACarChaosCarPawnSphere()
{

}

// Called when the game starts or when spawned
void ACarChaosCarPawnSphere::BeginPlay()
{
    Super::BeginPlay();
    PrimaryActorTick.bCanEverTick = true;

    CarCollision = FindComponentByClass<USphereComponent>();
    ArrowForward = FindComponentByClass<UArrowComponent>();
}

// Called every frame
void ACarChaosCarPawnSphere::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FRotator CurrentRotation = ArrowForward->GetComponentRotation();
    FRotator NewRotation(0.0f, 0.0f, CurrentRotation.Yaw);
    ArrowForward->SetWorldRotation(NewRotation);

    if (IsGrounded())
    {
        //Sideways Grip
        FVector Velocity = CarCollision->GetPhysicsLinearVelocity();

        FVector Forward = ArrowForward->GetForwardVector();
        FVector Right = ArrowForward->GetRightVector();

        float ForwardSpeed = FVector::DotProduct(Velocity, Forward);
        float SidewaysSpeed = FVector::DotProduct(Velocity, Right);

        FVector ForwardVelocity = Forward * ForwardSpeed;
        FVector SidewaysVelocity = Right * SidewaysSpeed;

        FVector NewVelocity = ForwardVelocity + (SidewaysVelocity * Grip);

        //CarCollision->SetPhysicsLinearVelocity(NewVelocity);

        //General Settings
        CarCollision->SetLinearDamping(0.6f);
        CarCollision->SetAngularDamping(8.f);
    }
    else
    {
        //General Settings
        CarCollision->SetLinearDamping(0.1f);
        CarCollision->SetAngularDamping(20.f);
    }

    //Downforce
    FVector Velocity = CarCollision->GetPhysicsLinearVelocity();
    float Speed = Velocity.Size();

    FVector Downforce = -FVector::UpVector * Speed * 2.5f;

    //CarCollision->AddForce(Downforce);

    //Stabilization trying to stay upright
    FVector Up = CarCollision->GetUpVector();
    FVector WorldUp = FVector::UpVector;

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Up: x: %f y: %f z: %f"), Up.X, Up.Y, Up.Z));

    FVector TorqueAxis = FVector::CrossProduct(Up, WorldUp);

    float UprightStrength = 50000000.f;
    FVector UprightTorque = TorqueAxis * UprightStrength;

    //CarCollision->AddTorqueInDegrees(UprightTorque);
}

void ACarChaosCarPawnSphere::UpdateGasBarValue()
{
    CurrentGas = FMath::Clamp(CurrentGas - (GasUsage * (GetWorld()->GetDeltaSeconds())), 0.f, MaxGas);
}

void ACarChaosCarPawnSphere::AddGas()
{
    CurrentGas = FMath::Clamp(CurrentGas + GasPickupValue, 0.f, MaxGas);
}

bool ACarChaosCarPawnSphere::IsGrounded()
{
    float TraceDistance = 41.f;
    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    FVector Start = CarCollision->GetComponentLocation();
    FVector End = Start - (FVector::UpVector * TraceDistance);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params)) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Up: x: true"));
        return true;
    }
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Up: x: false"));
    return false;
}

void ACarChaosCarPawnSphere::ChangeSpeed(float SpeedValue)
{
    if (!IsGrounded()) return;

    float DeltaTime = GetWorld()->GetDeltaSeconds();

    FVector Forward = ArrowForward->GetForwardVector();
    FVector FlatForward = FVector::VectorPlaneProject(Forward, FVector::UpVector).GetSafeNormal();

    if (SpeedValue > 0)
    {
        FVector AccelerationForce = FlatForward * AccelerationPower;
        CarCollision->AddForce(AccelerationForce);
    }
    else if (SpeedValue < 0)
    {
        FVector BrakingForce = FlatForward * BrakingPower;
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

void ACarChaosCarPawnSphere::Steer(float SteeringValue)
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

