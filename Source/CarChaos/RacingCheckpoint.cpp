// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingCheckpoint.h"
#include "CarChaosCarPawnPC.h"

ARacingCheckpoint::ARacingCheckpoint()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ARacingCheckpoint::BeginPlay()
{
	Super::BeginPlay();

	Collision = FindComponentByClass<UBoxComponent>();
    Collision->OnComponentBeginOverlap.AddDynamic(this, &ARacingCheckpoint::OnOverlap);
}

void ARacingCheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARacingCheckpoint::OnOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;
    if (!PlayerClass || !EnemyClass) return;

    if (OtherActor->IsA(PlayerClass))
    {
        ACarChaosCarPawnPC* Car = Cast<ACarChaosCarPawnPC>(OtherActor);
        Car->UpdateCheckpoint(CheckpointNumber);
    }
    if (OtherActor->IsA(EnemyClass))
    {
        // valid car
    }
}
