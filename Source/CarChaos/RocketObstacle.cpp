// Fill out your copyright notice in the Description page of Project Settings.

#include "RocketObstacle.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "CarChaosPlayerController.h"
#include <Kismet/GameplayStatics.h>

ARocketObstacle::ARocketObstacle()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ARocketObstacle::BeginPlay()
{
    Super::BeginPlay();

    Collision = FindComponentByClass<UCapsuleComponent>();
    Collision->OnComponentBeginOverlap.AddDynamic(this, &ARocketObstacle::OnOverlap);

    DirectionArrow = FindComponentByClass<UArrowComponent>();
}

void ARocketObstacle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector NewLocation = GetActorLocation() + (DirectionArrow->GetForwardVector() * FlyingSpeed * DeltaTime);
    SetActorLocation(NewLocation);

    AddActorLocalRotation(FRotator(0.f, 0.f, RotateSpeed * DeltaTime));

    CurrentDuration += DeltaTime;
    if (CurrentDuration >= MaxDuration)
    {
        Destroy();
    }
}

void ARocketObstacle::OnOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;
    if (OtherActor->IsA(CheckpointClass)) return;
    if (!CarClass) return;

    if (OtherActor->IsA(CarClass))
    {
        ACarChaosCarPawnPC* TempCar = Cast<ACarChaosCarPawnPC>(OtherActor);
        TempCar->StartOilSlow();
    }
    
    Destroy();
}
