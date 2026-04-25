// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarChaosCarPawnPC.h"
#include "OilObstacle.generated.h"

UCLASS()
class CARCHAOS_API AOilObstacle : public AActor
{
    GENERATED_BODY()

public:
    AOilObstacle();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    class USphereComponent* Collision;

    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    TSubclassOf<ACarChaosCarPawnPC> CarClass;

    UFUNCTION()
    void OnOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );
};
