// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitCameraMM.h"

// Sets default values
AOrbitCameraMM::AOrbitCameraMM()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOrbitCameraMM::BeginPlay()
{
	Super::BeginPlay();
	
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        PC->SetViewTarget(this);
    }
}

// Called every frame
void AOrbitCameraMM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!TargetActor) return;

    CurrentAngle += OrbitSpeed * DeltaTime;

    float Radians = FMath::DegreesToRadians(CurrentAngle);

    FVector TargetLocation = TargetActor->GetActorLocation();

    FVector Offset;
    Offset.X = FMath::Cos(Radians) * OrbitRadius;
    Offset.Y = FMath::Sin(Radians) * OrbitRadius;
    Offset.Z = OrbitHeight;

    FVector NewLocation = TargetLocation + Offset;

    FRotator LookAtRotation = (TargetLocation - NewLocation).Rotation();

    SetActorLocation(NewLocation);
    SetActorRotation(LookAtRotation);
}

