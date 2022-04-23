// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"
#include <numeric>
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>
#include <Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include "DrawDebugHelpers.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Sets default values
ABoid::ABoid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
	SetActorTickInterval(1.0f / 30.0f);

	InitTraceDirections(150);

	Neighbours.Reserve(NeighbourCount);
	Boids.Reserve(500);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	if (!SkeletalMesh->IsRegistered()) {
		SkeletalMesh->RegisterComponent();
	}
	SkeletalMesh->SetupAttachment(GetRootComponent());
	SkeletalMesh->SetComponentTickInterval(1.0f / 30.0f);
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoid::StaticClass(), Boids);

}

FVector ABoid::GetCohesionVector()
{
	if (Neighbours.Num() == 0) {
		return FVector(0.0f);
	}

	FVector cohesionVector = FVector(0.0f);
	for (AActor* neighbour : Neighbours) {
		if (neighbour->IsValidLowLevel()) {
			cohesionVector += neighbour->GetActorLocation();
		}
	}

	cohesionVector /= Neighbours.Num();

	cohesionVector = cohesionVector - GetActorLocation();
	cohesionVector.Normalize();

	/*DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + cohesionVector * 150.0f, 30.0f, FColor::Cyan, false, -1.0f, (uint8)0U, 5.0f);*/

	return cohesionVector;
}

FVector ABoid::GetAlignmentVector()
{
	if (Neighbours.Num() == 0) {
		return FVector(0.0f);
	}

	FVector alignmentVector = FVector(0.0f);
	for (AActor* neighbour : Neighbours) {
		if (neighbour->IsValidLowLevel()) {
			alignmentVector += neighbour->GetActorForwardVector();
		}
	}

	alignmentVector /= Neighbours.Num();
	alignmentVector.Normalize();

	/*DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + alignmentVector * 150.0f, 30.0f, FColor::Purple, false, -1.0f, (uint8)0U, 5.0f);*/

	return alignmentVector;
}

FVector ABoid::GetSeperationVector()
{
	if (Neighbours.Num() == 0) {
		return FVector(0.0f);
	}

	FVector seperationVector = FVector(0.0f);
	for (AActor* neighbour : Neighbours) {
		if (neighbour->IsValidLowLevel()) {
			float mult = SeperationDistance / FVector::Dist(neighbour->GetActorLocation(), GetActorLocation()) ;
			seperationVector += (neighbour->GetActorLocation() - GetActorLocation()) * mult;
		}
	}

	seperationVector /= Neighbours.Num();
	seperationVector *= -1.0f;
	seperationVector.Normalize();

	/*DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + seperationVector * 150.0f, 30.0f, FColor::Blue, false, -1.0f, (uint8)0U, 5.0f);*/

	return seperationVector;
}

void ABoid::InitTraceDirections(int TraceDirectionsCount)
{
	TraceDirections.Reserve(TraceDirectionsCount);

	for (int i = 0; i < TraceDirectionsCount; i++) {
		float t = (float)i / TraceDirectionsCount;
		float inclination = FMath::Acos(1.0f - 2.0f * t);
		float azimuth = AngleIncrement * i;

		float x = FMath::Sin(inclination) * FMath::Cos(azimuth);
		float y = FMath::Sin(inclination) * FMath::Sin(azimuth);
		float z = FMath::Cos(inclination);

		TraceDirections.Add(FVector(x, y, z));
	}
}

FVector ABoid::GetAvoidVector()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> ignoredActors;

	FHitResult result;

	FVector start = GetActorLocation();
	FVector end = start + GetActorForwardVector() * CollisionDetectionDistance;
	UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), start, end, CollisionAvoidanceRadius * 2.0f, objectTypes, false, ignoredActors, EDrawDebugTrace::Type::None, result, true);



	return result.ImpactNormal;
}

bool ABoid::TraceForward(FHitResult& OutHit)
{
	FVector start = GetActorLocation();
	FVector end = start + GetActorForwardVector() * CollisionDetectionDistance;

	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> ignoredActors;

	return UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), start, end, CollisionAvoidanceRadius, objectTypes, false, ignoredActors, EDrawDebugTrace::Type::None, OutHit, true);
}

void ABoid::CheckNeighbourDistances()
{
	for (int i = 0; i < Neighbours.Num(); i++) {
		if (FVector::Dist(GetActorLocation(), Neighbours[i]->GetActorLocation()) > DetectionRadius) {
			Neighbours.Remove(Neighbours[i]);
		}
	}

	if (Neighbours.Num() < NeighbourCount) {
		IsSearching = true;
	}
}

void ABoid::CheckBoidsDistances()
{
	if (Neighbours.Num() < NeighbourCount) {
		for (int i = 0; i < Boids.Num(); i++) {
			if (FVector::Dist(GetActorLocation(), Boids[i]->GetActorLocation()) < DetectionRadius) {
				if (Boids[i] != this) {
					if (Neighbours.Num() < NeighbourCount && !Neighbours.Contains(Boids[i])) {
						Neighbours.AddUnique(Boids[i]);
					}
					else if (Neighbours.Num() == NeighbourCount) {
						break;
					}
				}
			}
			else {
				if (Neighbours.Contains(Boids[i])) {
					Neighbours.Remove(Boids[i]);
				}
			}
		}
	}
	else {
		for (int i = 0; i < Neighbours.Num(); i++) {
			if (FVector::Dist(GetActorLocation(), Neighbours[i]->GetActorLocation()) > DetectionRadius) {
				Neighbours.RemoveAt(i, 1);
			}
		}
	}

}

void ABoid::StopAvoiding()
{
	IsAvoiding = false;
	CollisionAvoidVector = FVector(0.0f);
}

void ABoid::DrawDebug()
{
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckBoidsDistances();

	SkeletalMesh->SetWorldLocation(GetActorLocation());

	FVector dirVector = FVector(0.0f);
	dirVector += GetSeperationVector() * SeperationWeight;
	dirVector += GetCohesionVector() * CohesionWeight;
	dirVector += GetAlignmentVector() * AlignmentWeight;

	float movementMult = 1.0f;
	
	FHitResult hitResult;

	if (TraceForward(hitResult)) {
		IsAvoiding = true;
		CollisionAvoidVector = GetAvoidVector();

		GetWorld()->GetTimerManager().SetTimer(AvoidanceTimerHandle, this, &ABoid::StopAvoiding, CollisionAvoidTime, false);
	}

	if (IsAvoiding) {
		dirVector += CollisionAvoidVector * CollisionAvoidanceWeight;
		movementMult = 0.8f;
	}

	dirVector = UKismetMathLibrary::VInterpTo(UKismetMathLibrary::Conv_RotatorToVector(GetActorRotation()), dirVector, DeltaTime, TurnRate);

	FVector forwardMovement = GetActorLocation() + (GetActorForwardVector() * MoveSpeed * movementMult * DeltaTime);

	FRotator rotator = UKismetMathLibrary::Conv_VectorToRotator(dirVector);

	SetActorLocationAndRotation(forwardMovement, rotator);
}

