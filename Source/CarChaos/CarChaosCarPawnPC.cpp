// Fill out your copyright notice in the Description page of Project Settings.


#include "CarChaosRacingGameState.h"
#include "CarChaosCarPawnPC.h"

// Sets default values
ACarChaosCarPawnPC::ACarChaosCarPawnPC()
{

}

// Called when the game starts or when spawned
void ACarChaosCarPawnPC::BeginPlay()
{
    Super::BeginPlay();
    PrimaryActorTick.bCanEverTick = true;

    CarBodyMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("body")));
    DirectionArrow = FindComponentByClass<UArrowComponent>();

    TArray<UStaticMeshComponent*> Components;
    GetComponents<UStaticMeshComponent>(Components);

    for (UStaticMeshComponent* Comp : Components)
    {
        if (Comp->GetName().Contains(TEXT("wheel")))
        {
            WheelMeshs.Add(Comp);
        }
    }

    TimeRemaining = TimeLimit;

    GameState = GetWorld() ? GetWorld()->GetGameState<ACarChaosRacingGameState>() : nullptr;
}

// Called every frame
void ACarChaosCarPawnPC::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (IsGrounded())
    {
        //Sideways Grip
        FVector Velocity = CarBodyMesh->GetPhysicsLinearVelocity();

        FVector Forward = DirectionArrow->GetForwardVector();
        FVector Right = DirectionArrow->GetRightVector();

        float ForwardSpeed = FVector::DotProduct(Velocity, Forward);
        float SidewaysSpeed = FVector::DotProduct(Velocity, Right);

        FVector ForwardVelocity = Forward * ForwardSpeed;
        FVector SidewaysVelocity = Right * SidewaysSpeed;

        FVector NewVelocity = ForwardVelocity + (SidewaysVelocity * Grip);

        CarBodyMesh->SetPhysicsLinearVelocity(NewVelocity);

        //General Settings
        CarBodyMesh->SetLinearDamping(0.6f);
        CarBodyMesh->SetAngularDamping(8.f);
    }
    else
    {
        //General Settings
        CarBodyMesh->SetLinearDamping(0.1f);
        CarBodyMesh->SetAngularDamping(20.f);
    }

    //Downforce
    FVector Velocity = CarBodyMesh->GetPhysicsLinearVelocity();
    float Speed = Velocity.Size();

    FVector Downforce = -FVector::UpVector * Speed * 2.5f;

    CarBodyMesh->AddForce(Downforce);

    //Stabilization trying to stay upright
    FVector Up = DirectionArrow->GetUpVector();
    FVector WorldUp = FVector::UpVector;

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Up: x: %f y: %f z: %f"), Up.X, Up.Y, Up.Z));

    FVector TorqueAxis = FVector::CrossProduct(Up, WorldUp);

    float UprightStrength = 50000000.f;
    FVector UprightTorque = TorqueAxis * UprightStrength;

    CarBodyMesh->AddTorqueInDegrees(UprightTorque);

    //Only update points and time if pawn is player. Enemies dont need that for now. Other functions wont be executed if its an enemy. 
    if (!IsPlayer) return;

    //Gas
    UpdateGasBarValue();

    //Points
    CurrentPoints += (PointsPerSecond * DeltaTime);

    //Time and Gas check
    TimeRemaining = FMath::Clamp(TimeRemaining - DeltaTime, 0.f, TimeLimit);

    if (TimeRemaining <= 0.f || CurrentGas <= 0.f)
    {   
        GameState->LoseRace();
    }
}

void ACarChaosCarPawnPC::UpdateCheckpoint(int CheckpointNumber)
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Current Round %d"), RoundsDone));
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Current Checkpoint number %d"), CurrentCheckpoint));
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Checkpoint passed with number %d"), CheckpointNumber));

    if (CheckpointNumber > CurrentCheckpoint && CheckpointNumber - CurrentCheckpoint <= 2) {
        CurrentCheckpoint = CheckpointNumber;
    }
    else if (CheckpointNumber == 1 && (CurrentCheckpoint == MaxCheckpoints || CurrentCheckpoint == MaxCheckpoints - 1))
    {
        RoundsDone++;
        CurrentCheckpoint = 1;
    }

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("New Round %d"), RoundsDone));
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("New Checkpoint number %d"), CurrentCheckpoint));

    if (RoundsDone == 3) 
    {
        GameState->FinishRace();
    }
}

void ACarChaosCarPawnPC::UpdateGasBarValue()
{
    CurrentGas = FMath::Clamp(CurrentGas - (GasUsage * (GetWorld()->GetDeltaSeconds())), 0.f, MaxGas);
}

void ACarChaosCarPawnPC::AddGas()
{
    CurrentGas = FMath::Clamp(CurrentGas + GasPickupValue, 0.f, MaxGas);
    CurrentPoints += PointsPerPickup;
}

bool ACarChaosCarPawnPC::IsGrounded()
{
    int GroundedWheels = 0;
    int OffroadWheels = 0;
    float TraceDistance = 30.f;
    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    for (UStaticMeshComponent* Wheel : WheelMeshs)
    {
        FVector Start = Wheel->GetComponentLocation();
        FVector End = Start - (FVector::UpVector * TraceDistance);

        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
        {
            GroundedWheels++;

            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Hit.GetActor()->GetActorLabel());

            if (Hit.GetActor()->GetActorLabel().Contains(TEXT("grass")) || Hit.GetActor()->GetActorLabel().Contains(TEXT("Sand")))
            {
                OffroadWheels++;
            }
        }
    }

    if (GroundedWheels == 0 || OffroadWheels == 0)
    {
        CurrentOffroadValue = 0.f;
    }
    else
    {
        CurrentOffroadValue = (float) OffroadWheels / (float) GroundedWheels;
    }

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Up: x: %d"), GroundedWheels));

    return GroundedWheels >= 2;
}

void ACarChaosCarPawnPC::ChangeSpeed(float SpeedValue)
{
    if (!IsGrounded()) return;

    float DeltaTime = GetWorld()->GetDeltaSeconds();

    FVector Forward = DirectionArrow->GetForwardVector();
    FVector FlatForward = FVector::VectorPlaneProject(Forward, FVector::UpVector).GetSafeNormal();

    if (SpeedValue > 0)
    {
        FVector AccelerationForce = FlatForward * (AccelerationPower * (1 - (OffroadPenalty * CurrentOffroadValue)));
        CarBodyMesh->AddForce(AccelerationForce);
    }
    else if (SpeedValue < 0)
    {
        FVector BrakingForce = FlatForward * (BrakingPower * (1 - (OffroadPenalty * CurrentOffroadValue)));
        CarBodyMesh->AddForce(BrakingForce);
    }
    else
    {
        FVector CurrentVelocity = CarBodyMesh->GetPhysicsLinearVelocity();
        if (CurrentVelocity.Size() < 5.f)
        {
            CarBodyMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
        }
    }
}

void ACarChaosCarPawnPC::Steer(float SteeringValue)
{
    FVector Forward = DirectionArrow->GetForwardVector();
    FVector Velocity = CarBodyMesh->GetPhysicsLinearVelocity();
    float ForwardSpeed = FVector::DotProduct(Velocity, Forward);

    if (ForwardSpeed < 0.f)
    {
        SteeringValue = -SteeringValue;
    }

    float DeltaTime = GetWorld()->GetDeltaSeconds();

    // Smooth steering input
    CurrentSteering = FMath::FInterpTo(CurrentSteering, SteeringValue, DeltaTime, 5.0f);

    if (!IsGrounded())
    {
        CurrentSteering *= 0.8f;
    }

    float TurnTorque = CurrentSteering * SteeringStrength;

    FVector Torque(0.f, 0.f, TurnTorque);
    CarBodyMesh->AddTorqueInDegrees(Torque);
}

