// Fill out your copyright notice in the Description page of Project Settings.

#include "RocketObstacle.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "CarChaosCarPawnPC.h"
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

    DirectionArrow = FindComponentByClass<UArrowComponent>();

    ExplosionNiagaraComponent = FindComponentByClass<UNiagaraComponent>();
}

void ARocketObstacle::Activate()
{
    Collision->OnComponentBeginOverlap.AddDynamic(this, &ARocketObstacle::OnOverlap);
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
        TempCar->StartRocketSlow();

        UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation(), 4.f);

        CarRef->RocketHit();
    }
    
    TArray<UActorComponent*> Components;
    GetComponents(Components);

    for (UActorComponent* Comp : Components)
    {
        if (Comp != ExplosionNiagaraComponent)
        {
            if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
            {
                Prim->SetHiddenInGame(true);
                Prim->SetVisibility(false);
                Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
        }
    }

    ExplosionNiagaraComponent->SetWorldRotation(FRotator::ZeroRotator);
    ExplosionNiagaraComponent->SetHiddenInGame(false);
    ExplosionNiagaraComponent->SetVisibility(true);
    ExplosionNiagaraComponent->Activate(true);

    GetWorldTimerManager().SetTimer(
        ExplosionTimer,
        this,
        &ARocketObstacle::DeleteRocket,
        ExplosionTime,
        false
    );
}

void ARocketObstacle::DeleteRocket()
{
    Destroy();
}
