// Fill out your copyright notice in the Description page of Project Settings.


#include "CarChaosRacingGameState.h"
#include "RacingCheckpoint.h"
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

    if (IsPlayer)
    {
        //Only update points and time if pawn is player. Enemies dont need that for now. Other functions wont be executed if its an enemy. 
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
    else
    {
        //Only do AI navigation on enemies
        int AIVersion = 2;
        if (AIVersion == 1)
        {
            FVector TargetLocation = NextCheckpointObject->GetActorLocation();

            FVector ToTarget = (TargetLocation - CarBodyMesh->GetComponentLocation()).GetSafeNormal();
            FVector Forward = DirectionArrow->GetForwardVector();
            float SteeringInput = FVector::CrossProduct(Forward, ToTarget).Z;
            Steer(SteeringInput * 2.f);

            float Alignment = FVector::DotProduct(Forward, ToTarget);
            float SpeedInput = 0.f;
            if (Alignment > 0.7f)
            {
                SpeedInput = 0.7f; // full throttle
            }
            else if (Alignment > 0.3f)
            {
                SpeedInput = 0.35f; // slow down for turn
            }
            else
            {
                SpeedInput = 0.f; // too sharp, stop accelerating
            }
            ChangeSpeed(SpeedInput);
        }
        else if (AIVersion == 2)
        {
            if (!RacingSpline) return;

            float ClosestKey = RacingSpline->FindInputKeyClosestToWorldLocation(CarBodyMesh->GetComponentLocation());
            CurrentSplineDistance = RacingSpline->GetDistanceAlongSplineAtSplineInputKey(ClosestKey);

            float TargetDistance = CurrentSplineDistance + LookAheadDistance;
            FVector TargetLocation = RacingSpline->GetLocationAtDistanceAlongSpline(TargetDistance, ESplineCoordinateSpace::World);

            FVector Forward = DirectionArrow->GetForwardVector();
            FVector ToTarget = (TargetLocation - CarBodyMesh->GetComponentLocation()).GetSafeNormal();

            float SteeringInput = FVector::CrossProduct(Forward, ToTarget).Z;
            SteeringInput = FMath::Clamp(SteeringInput, -1.f, 1.f);
            Steer(SteeringInput);

            FVector NearPoint = RacingSpline->GetLocationAtDistanceAlongSpline(CurrentSplineDistance + 300.f, ESplineCoordinateSpace::World);
            FVector FarPoint = RacingSpline->GetLocationAtDistanceAlongSpline(CurrentSplineDistance + 1000.f, ESplineCoordinateSpace::World);

            FVector Dir1 = (NearPoint - CarBodyMesh->GetComponentLocation()).GetSafeNormal();
            FVector Dir2 = (FarPoint - NearPoint).GetSafeNormal();

            float CurveFactor = FVector::DotProduct(Dir1, Dir2);

            float SpeedInput = FMath::Clamp(CurveFactor, 0.3f, 1.f);
            ChangeSpeed(SpeedInput);
        }
    }
}

void ACarChaosCarPawnPC::UpdateCheckpoint(int CheckpointNumber)
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Current Round %d"), RoundsDone));
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Current Checkpoint number %d"), CurrentCheckpoint));
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Checkpoint passed with number %d"), CheckpointNumber));

    if (CheckpointNumber > CurrentCheckpoint && CheckpointNumber - CurrentCheckpoint <= 2) {
        CurrentCheckpoint = CheckpointNumber;
        if (CurrentCheckpoint == MaxCheckpoints)
        {
            NextCheckpointObject = GameState->Checkpoints[0];
        }
        else
        {
            NextCheckpointObject = GameState->Checkpoints[CurrentCheckpoint];
        }
    }
    else if (CheckpointNumber == 1 && (CurrentCheckpoint == MaxCheckpoints || CurrentCheckpoint == MaxCheckpoints - 1))
    {
        RoundsDone++;
        CurrentCheckpoint = 1;
        NextCheckpointObject = GameState->Checkpoints[CurrentCheckpoint];
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
        CarBodyMesh->AddForce(AccelerationForce * SpeedValue);
    }
    else if (SpeedValue < 0)
    {
        FVector BrakingForce = FlatForward * (BrakingPower * (1 - (OffroadPenalty * CurrentOffroadValue)));
        CarBodyMesh->AddForce(BrakingForce * -SpeedValue);
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

