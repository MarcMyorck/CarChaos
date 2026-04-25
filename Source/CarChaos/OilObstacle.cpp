// Fill out your copyright notice in the Description page of Project Settings.

#include "OilObstacle.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "CarChaosPlayerController.h"
#include <Kismet/GameplayStatics.h>

AOilObstacle::AOilObstacle()
{
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;

    Collision->SetSphereRadius(60.f);
    Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
}

void AOilObstacle::BeginPlay()
{
    Super::BeginPlay();

    Collision->OnComponentBeginOverlap.AddDynamic(this, &AOilObstacle::OnOverlap);
}

void AOilObstacle::OnOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;
    if (!CarClass) return;

    if (OtherActor->IsA(CarClass))
    {
        ACarChaosCarPawnPC* TempCar = Cast<ACarChaosCarPawnPC>(OtherActor);
        TempCar->OilSpin();

        Destroy();
    }
}

