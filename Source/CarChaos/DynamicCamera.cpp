#include "DynamicCamera.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

UDynamicCamera::UDynamicCamera()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDynamicCamera::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = Cast<ACarChaosCarPawnPC>(GetOwner());
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("CarCameraController: Owner not a Pawn!"));
		return;
	}

	if (!SpringArm)
	{
		SpringArm = OwnerPawn->FindComponentByClass<USpringArmComponent>();
	}

	if (!Camera && SpringArm)
	{
		Camera = OwnerPawn->FindComponentByClass<UCameraComponent>();
	}
}

void UDynamicCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerPawn || !SpringArm || !Camera) return;

	// Speed-based FOV
	FVector Velocity = OwnerPawn->CarBodyMesh->GetPhysicsLinearVelocity();
	float ForwardSpeed = FVector::DotProduct(Velocity, OwnerPawn->DirectionArrow->GetForwardVector());
	float AbsSpeed = FMath::Abs(ForwardSpeed);
	float NormalizedSpeed = FMath::Clamp(AbsSpeed / 2050.f, 0.f, 1.f);
	float TargetFOV = FMath::Lerp(MinFOV, MaxFOV, NormalizedSpeed);
	float CurrentFOV = Camera->FieldOfView;
	Camera->SetFieldOfView(FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, FOVInterpSpeed));

	// Dynamic camera distance
	float TargetArmLength = FMath::Lerp(MinArmLength, MaxArmLength, NormalizedSpeed);
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetArmLength, DeltaTime, ArmInterpSpeed);

	// Look-ahead rotation
	if (!Velocity.IsNearlyZero())
	{
		FVector ForwardDir = OwnerPawn->DirectionArrow->GetForwardVector();
		FRotator TargetRotation = ForwardDir.Rotation();

		FVector LookAheadOffset = ForwardDir * LookAheadDistance;
		FVector CameraTargetLocation = OwnerPawn->CarBodyMesh->GetComponentLocation() + LookAheadOffset;
		SpringArm->SetWorldRotation(FMath::RInterpTo(SpringArm->GetComponentRotation(), TargetRotation, DeltaTime, 3.f));
	}

	// Side lean in corners
	float TargetRoll = -OwnerPawn->CurrentSteering * MaxSideLean;
	FRotator CurrentRotation = SpringArm->GetComponentRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, TargetRoll), DeltaTime, SideLeanInterpSpeed);

	SpringArm->SetWorldRotation(NewRotation);
}