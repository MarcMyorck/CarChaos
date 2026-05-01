// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarChaosCarPawnPC.h"
#include "RacingCheckpoint.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "RocketObstacle.generated.h"

UCLASS()
class CARCHAOS_API ARocketObstacle : public AActor
{
    GENERATED_BODY()

public:
    ARocketObstacle();

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    UCapsuleComponent* Collision;

    UPROPERTY(VisibleAnywhere)
    UArrowComponent* DirectionArrow;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    float FlyingSpeed = 2500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Duration")
    float CurrentDuration = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Duration")
    float MaxDuration = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCheck")
    TSubclassOf<ACarChaosCarPawnPC> CarClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCheck")
    TSubclassOf<ARacingCheckpoint> CheckpointClass;

    UFUNCTION()
    void OnOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    float ExplosionTime = 2.f;

    FTimerHandle ExplosionTimer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    UNiagaraComponent* ExplosionNiagaraComponent;

    UFUNCTION(BlueprintCallable, Category = "Delete")
    void DeleteRocket();

    UPROPERTY(EditAnywhere, Category = "Animation")
    float RotateSpeed = 180.f;
};
