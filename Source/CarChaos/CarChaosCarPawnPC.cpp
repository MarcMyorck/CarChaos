// Fill out your copyright notice in the Description page of Project Settings.


#include "CarChaosCarPawnPC.h"
#include "CarChaosRacingGameState.h"
#include "RacingCheckpoint.h"
#include <Kismet/GameplayStatics.h>

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

    CarCollisionCapsule = Cast<UCapsuleComponent>(GetDefaultSubobjectByName(TEXT("BumpOffHitbox")));
    CarCollisionCapsule->OnComponentHit.AddDynamic(this, &ACarChaosCarPawnPC::OnCarHit);

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

    LastPosition = CarBodyMesh->GetComponentLocation();

    if (AccelerateSound)
    {
        AccelerateAudioComponent = UGameplayStatics::SpawnSoundAttached(
            AccelerateSound,
            CarBodyMesh,
            NAME_None,
            FVector::ZeroVector,
            EAttachLocation::KeepRelativeOffset,
            false, 
            0.0f
        );

        AccelerateAudioComponent->bIsUISound = true;
    }

    if (BrakeSound)
    {
        BrakeAudioComponent = UGameplayStatics::SpawnSoundAttached(
            BrakeSound,
            CarBodyMesh,
            NAME_None,
            FVector::ZeroVector,
            EAttachLocation::KeepRelativeOffset,
            true,
            0.0f
        );

        BrakeAudioComponent->bIsUISound = true;
    }

    if (SustainSound)
    {
        SustainAudioComponent = UGameplayStatics::SpawnSoundAttached(
            SustainSound,
            CarBodyMesh,
            NAME_None,
            FVector::ZeroVector,
            EAttachLocation::KeepRelativeOffset,
            true,
            0.1f
        );

        SustainAudioComponent->bIsUISound = true;
    }

    if (GrassSound)
    {
        GrassAudioComponent = UGameplayStatics::SpawnSoundAttached(
            GrassSound,
            CarBodyMesh,
            NAME_None,
            FVector::ZeroVector,
            EAttachLocation::KeepRelativeOffset,
            true,
            0.0f
        );

        GrassAudioComponent->bIsUISound = true;
    }
}

// Called every frame
void ACarChaosCarPawnPC::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (IsPlayer) UpdateCarSounds();

    if (IsInStarting) return;

    if (IsGrounded())
    {
        if (!IsOilSlowed)
        {
            //Sideways Grip (Not during Oil Slow to make car feel slippery)
            FVector Velocity = CarBodyMesh->GetPhysicsLinearVelocity();

            FVector Forward = DirectionArrow->GetForwardVector();
            FVector Right = DirectionArrow->GetRightVector();

            float ForwardSpeed = FVector::DotProduct(Velocity, Forward);
            float SidewaysSpeed = FVector::DotProduct(Velocity, Right);

            FVector ForwardVelocity = Forward * ForwardSpeed;
            FVector SidewaysVelocity = Right * SidewaysSpeed;

            FVector NewVelocity = ForwardVelocity + (SidewaysVelocity * Grip);

            CarBodyMesh->SetPhysicsLinearVelocity(NewVelocity);
        }

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
        CurrentPoints += ((PointsPerSecond * (4.f / (float) CurrentPosition)) * DeltaTime);

        //Time and Gas check
        TimeRemaining = FMath::Clamp(TimeRemaining - DeltaTime, 0.f, TimeLimit);

        if (TimeRemaining <= 0.f || CurrentGas <= 0.f)
        {
            IsInputEnabled = false;
            IsRaceFinished = true;
            GameState->FinishRace(false);
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

            if (StuckTimer >= 3.f)
            {
                RecoveryTimer = 0.f;
            }
            if (StuckTimer >= 5.f)
            {
                FVector SafeLocation = RacingSpline->GetLocationAtDistanceAlongSpline(CurrentSplineDistance, ESplineCoordinateSpace::World);
                SetActorLocation(SafeLocation + FVector(0, 0, 50));
                StuckTimer = 0.f;
                RecoveryTimer = 0.5f;
            }

            if (RecoveryTimer < 0.25f)
            {
                RecoveryTimer += DeltaTime;
                ChangeSpeed(-1.f);

                FVector CarForward = DirectionArrow->GetForwardVector();
                FVector VectorToPoint = RacingSpline->GetLocationAtDistanceAlongSpline(CurrentSplineDistance, ESplineCoordinateSpace::World) - CarBodyMesh->GetComponentLocation();
                FVector ToPointNormalized = VectorToPoint.GetSafeNormal();
                float CrossProductResult = FVector::CrossProduct(CarForward, ToPointNormalized).Z;

                Steer(-CrossProductResult);

                if (RecoveryTimer >= 0.25f)
                {
                    CurrentSpeedInput = 0.f;
                    CurrentSteering = 0.f;
                }
            }
            else
            {
                FVector NearPoint = RacingSpline->GetLocationAtDistanceAlongSpline(CurrentSplineDistance + LookAheadDistance / 3, ESplineCoordinateSpace::World);
                FVector FarPoint = RacingSpline->GetLocationAtDistanceAlongSpline(CurrentSplineDistance + LookAheadDistance, ESplineCoordinateSpace::World);

                FVector Dir1 = (NearPoint - CarBodyMesh->GetComponentLocation()).GetSafeNormal();
                FVector Dir2 = (FarPoint - NearPoint).GetSafeNormal();

                float CurveFactor = FVector::DotProduct(Dir1, Dir2);

                float AIBaseSpeedFactor = 0.95f;
                float AIPositionSpeedBonusFactor = 0.15f;

                float SpeedInput = FMath::Clamp(CurveFactor, 0.1f, 1.f);
                ChangeSpeed(SpeedInput * (AIBaseSpeedFactor + AIPositionSpeedBonusFactor * CurrentPosition));

                float ClosestKey = RacingSpline->FindInputKeyClosestToWorldLocation(CarBodyMesh->GetComponentLocation());
                CurrentSplineDistance = RacingSpline->GetDistanceAlongSplineAtSplineInputKey(ClosestKey);

                float TargetDistance = CurrentSplineDistance + LookAheadDistance;
                FVector TargetLocation = RacingSpline->GetLocationAtDistanceAlongSpline(TargetDistance, ESplineCoordinateSpace::World);

                FVector Forward = DirectionArrow->GetForwardVector();
                FVector ToTarget = (TargetLocation - CarBodyMesh->GetComponentLocation()).GetSafeNormal();

                float SteeringInput = FVector::CrossProduct(Forward, ToTarget).Z;
                SteeringInput = FMath::Clamp(SteeringInput, -1.f, 1.f);
                float AISteeringBonusFactor = 1.8f;
                Steer(SteeringInput * AISteeringBonusFactor);

                float DropRoll = FMath::FRand();
                float AverageSecondsPerDrop = 40.f;
                float ReducedSecondsPerPosition = 4.f;
                if (DropRoll < (DeltaTime / (AverageSecondsPerDrop - ReducedSecondsPerPosition * CurrentPosition)))
                {
                    CurrentGas = 100.f;

                    float DropRoll2 = FMath::FRand();
                    if (DropRoll2 < 0.5f)
                    {
                        DropOil();
                    }
                    else
                    {
                        DropRocket();
                    }
                }
            }

            float MovementDistance = FVector::Dist(CarBodyMesh->GetComponentLocation(), LastPosition);

            if (MovementDistance < 5.f)
            {
                StuckTimer += DeltaTime;
            }
            else
            {
                StuckTimer = 0.f;
            }

            LastPosition = CarBodyMesh->GetComponentLocation();
        }
    }

    if (IsOilSlowed)
    {
        OilSlowTimer += DeltaTime;

        if (OilSlowTimer >= OilSlowDuration)
        {
            IsOilSlowed = false;

            for (UStaticMeshComponent* WheelMesh : WheelMeshs)
            {
                WheelMesh->SetMaterial(0, OriginalTireMat);
            }
        }
    }
    if (IsRocketSlowed)
    {
        RocketSlowTimer += DeltaTime;

        if (RocketSlowTimer >= RocketSlowDuration)
        {
            IsRocketSlowed = false;

            for (UStaticMeshComponent* WheelMesh : WheelMeshs)
            {
                WheelMesh->SetMaterial(0, OriginalTireMat);
            }
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
        if (IsPlayer)
        {
            IsInputEnabled = false;
            IsRaceFinished = true;
            GameState->FinishRace(true);
        }
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

            if (Hit.GetActor()->ActorHasTag(TEXT("offroad")))
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
    if (IsInStarting) return;
    if (!IsInputEnabled) return;
    if (!IsGrounded()) return;
    if (IsRocketSlowed) return;

    float DeltaTime = GetWorld()->GetDeltaSeconds();

    FVector Forward = DirectionArrow->GetForwardVector();
    FVector FlatForward = FVector::VectorPlaneProject(Forward, FVector::UpVector).GetSafeNormal();

    if (SpeedValue > 0)
    {
        FVector AccelerationForce = (FlatForward * (AccelerationPower * (1 - (OffroadPenalty * CurrentOffroadValue)))) * SpeedValue;

        if (IsOilSlowed)
        {
            //Small Slowdown during Oil
            AccelerationForce = AccelerationForce * OilSlowFactor;
        }

        CarBodyMesh->AddForce(AccelerationForce);
    }
    else if (SpeedValue < 0)
    {
        FVector BrakingForce = (FlatForward * (BrakingPower * (1 - (OffroadPenalty * CurrentOffroadValue)))) * -SpeedValue;

        if (IsOilSlowed)
        {
            //No braking during Oil because of no friction
            BrakingForce = FVector::ZeroVector;
        }

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

    CurrentSpeedInput = SpeedValue;
}

void ACarChaosCarPawnPC::Steer(float SteeringValue)
{
    if (IsInStarting) return;
    if (!IsInputEnabled) return;

    FVector Forward = DirectionArrow->GetForwardVector();
    FVector Velocity = CarBodyMesh->GetPhysicsLinearVelocity();
    float ForwardSpeed = FVector::DotProduct(Velocity, Forward);

    if (ForwardSpeed < 0.f && CurrentSpeedInput < 0.f)
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

void ACarChaosCarPawnPC::OnCarHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this)
    {
        ACarChaosCarPawnPC* OtherCar = Cast<ACarChaosCarPawnPC>(OtherActor);
        if (OtherCar)
        {
            FVector HitNormal = Hit.ImpactNormal;
            FVector CarVelocity = CarBodyMesh->GetPhysicsLinearVelocity();
            FVector OtherCarVelocity = OtherCar->CarBodyMesh->GetPhysicsLinearVelocity();

            FVector RelativeVelocity = CarVelocity - OtherCarVelocity;
            float RelativeSpeed = FVector::DotProduct(RelativeVelocity, HitNormal);
            if (RelativeSpeed < 0)
            {
                FVector BounceForce = HitNormal * BounceOffStrength;

                CarBodyMesh->AddForce(BounceForce);
                OtherCar->CarBodyMesh->AddForce(-BounceForce);
            }
        }
    }
}

void ACarChaosCarPawnPC::DropOil()
{
    if (!IsInputEnabled) return;

    if (CurrentGas >= DropOilCost && IsGrounded())
    {
        CurrentGas -= DropOilCost;

        UWorld* World = GetWorld();
        FVector SpawnLocation = CarBodyMesh->GetComponentLocation() - (DirectionArrow->GetForwardVector() * 200.f);
        FRotator SpawnRotation(0.f, FMath::RandRange(0.0f, 360.0f), 0.f);
        FActorSpawnParameters SpawnParams;

        AActor* SpawnedActor = World->SpawnActor<AActor>(
            OilObstacleBlueprintClass,
            SpawnLocation,
            SpawnRotation,
            SpawnParams
        );
    }
}

void ACarChaosCarPawnPC::StartOilSlow()
{
    IsOilSlowed = true;
    OilSlowTimer = 0.f;

    for (UStaticMeshComponent* WheelMesh : WheelMeshs)
    {
        WheelMesh->SetMaterial(0, OilTireMat);
    }
}

void ACarChaosCarPawnPC::DropRocket()
{
    if (!IsInputEnabled) return;

    if (CurrentGas >= DropRocketCost)
    {
        CurrentGas -= DropRocketCost;

        UWorld* World = GetWorld();
        FVector SpawnLocation = CarBodyMesh->GetComponentLocation() + (DirectionArrow->GetForwardVector() * 200.f);
        SpawnLocation.Z += 40.f;
        FRotator SpawnRotation = DirectionArrow->GetForwardVector().Rotation();
        FActorSpawnParameters SpawnParams;

        AActor* SpawnedActor = World->SpawnActor<AActor>(
            RocketObstacleBlueprintClass,
            SpawnLocation,
            SpawnRotation,
            SpawnParams
        );
    }
}

void ACarChaosCarPawnPC::StartRocketSlow()
{
    IsRocketSlowed = true;
    RocketSlowTimer = 0.f;

    for (UStaticMeshComponent* WheelMesh : WheelMeshs)
    {
        WheelMesh->SetMaterial(0, RocketTireMat);
    }
}

void ACarChaosCarPawnPC::UpdateCarSounds()
{
    FVector Velocity = CarBodyMesh->GetPhysicsLinearVelocity();
    float ForwardSpeed = FVector::DotProduct(Velocity, DirectionArrow->GetForwardVector());
    float AbsSpeed = FMath::Abs(ForwardSpeed);

    if(IsPlayer) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("AbsSpeed: %f"), AbsSpeed));

    if (CurrentSpeedInput > 0 && FMath::IsNearlyZero(ForwardSpeed, 5.f))
    {
        if (AccelerateAudioComponent && !AccelerateSoundPlayed)
        {
            AccelerateAudioComponent->SetVolumeMultiplier(FMath::Clamp(CurrentSpeedInput, 0.f, 1.f));
            if (!AccelerateAudioComponent->IsPlaying())
            {
                AccelerateAudioComponent->Play();
                AccelerateSoundPlayed = true;
            }
        }
    }
    else
    {
        if (AccelerateAudioComponent && AccelerateAudioComponent->IsPlaying())
        {
            AccelerateSoundPlayed = false;
        }
    }

    if ((ForwardSpeed > 0.f && CurrentSpeedInput < 0) || (ForwardSpeed > 100.f && CurrentSpeedInput == 0))
    {
        if (BrakeAudioComponent)
        {
            BrakeAudioComponent->SetVolumeMultiplier(FMath::Clamp(FMath::Abs(CurrentSpeedInput), 0.f, 1.f));
            if (!BrakeAudioComponent->IsPlaying()) BrakeAudioComponent->Play();
        }
    }
    else
    {
        if (BrakeAudioComponent && BrakeAudioComponent->IsPlaying())
        {
            BrakeAudioComponent->SetVolumeMultiplier(0.f);
        }
    }

    if (SustainAudioComponent)
    {
        float Volume = FMath::Clamp(AbsSpeed / 2050.f, 0.05f, 1.f);
        SustainAudioComponent->SetVolumeMultiplier(Volume);
        SustainAudioComponent->SetPitchMultiplier(FMath::Clamp(0.8f + (AbsSpeed / 3075.f), 0.8f, 2.0f));
        if (!SustainAudioComponent->IsPlaying()) SustainAudioComponent->Play();
    }

    if (GrassAudioComponent)
    {
        GrassAudioComponent->SetVolumeMultiplier(CurrentOffroadValue);
        if (!GrassAudioComponent->IsPlaying()) GrassAudioComponent->Play();
    }
}
