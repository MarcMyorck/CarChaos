// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarChaosCarPawnPC.h"
#include "GasPickup.generated.h"

UCLASS()
class CARCHAOS_API AGasPickup : public AActor
{
    GENERATED_BODY()

public:
    AGasPickup();

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    class USphereComponent* Collision;

    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere, Category = "Pickup")
    float FuelAmount = 25.f;

    UPROPERTY(EditAnywhere, Category = "Pickup")
    float RespawnTime = 5.f;

    FTimerHandle RespawnTimer;

    UPROPERTY(EditAnywhere)
    TSubclassOf<ACarChaosCarPawnPC> PlayerClass;

    UPROPERTY(EditAnywhere) //Change when enemy exists
    TSubclassOf<ACarChaosCarPawnPC> EnemyClass;

    UFUNCTION()
    void OnOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    void RespawnPickup();

    FVector StartLocation;

    UPROPERTY(EditAnywhere, Category = "Pickup|Animation")
    float BobHeight = 20.f;

    UPROPERTY(EditAnywhere, Category = "Pickup|Animation")
    float BobSpeed = 2.f;

    float RunningTime = 0.f;

    UPROPERTY(EditAnywhere, Category = "Pickup|Animation")
    float RotateSpeed = 90.f;
};
