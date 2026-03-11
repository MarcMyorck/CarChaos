// Fill out your copyright notice in the Description page of Project Settings.

#include "GasPickup.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "CarChaosPlayerController.h"
#include <Kismet/GameplayStatics.h>

AGasPickup::AGasPickup()
{
    PrimaryActorTick.bCanEverTick = true;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;

    Collision->SetSphereRadius(100.f);
    Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
}

void AGasPickup::BeginPlay()
{
    Super::BeginPlay();

    StartLocation = GetActorLocation();

    Collision->OnComponentBeginOverlap.AddDynamic(this, &AGasPickup::OnOverlap);
}

void AGasPickup::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    RunningTime += DeltaTime;

    float Offset = FMath::Sin(RunningTime * BobSpeed) * BobHeight;

    FVector NewLocation = StartLocation + FVector(0, 0, Offset);
    SetActorLocation(NewLocation);

    AddActorLocalRotation(FRotator(0.f, RotateSpeed * DeltaTime, 0.f));
}

void AGasPickup::OnOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;

    ACarChaosPlayerController* PC = Cast<ACarChaosPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (PC)
    {
         PC->AddGas();
    }

    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);

    GetWorldTimerManager().SetTimer(
        RespawnTimer,
        this,
        &AGasPickup::RespawnPickup,
        RespawnTime,
        false
    );
}

void AGasPickup::RespawnPickup()
{
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
}

