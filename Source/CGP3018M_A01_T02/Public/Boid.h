// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

#include "Boid.generated.h"

UCLASS()
class CGP3018M_A01_T02_API ABoid : public AActor
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere)
	float DetectionRadius = 500.0f;
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 500.0f;
	UPROPERTY(EditAnywhere)
	float TurnRate = 0.5f;
	UPROPERTY(EditAnywhere)
	float SeperationDistance = 40.0f;
	UPROPERTY(EditAnywhere)
	float CollisionAvoidanceRadius = 25.0f;
	UPROPERTY(EditAnywhere)
	float CollisionDetectionDistance = 500.0f;

	UPROPERTY(EditAnywhere)
	float CohesionWeight = 1.0f;
	UPROPERTY(EditAnywhere)
	float SeperationWeight = 1.0f;
	UPROPERTY(EditAnywhere)
	float AlignmentWeight = 1.0f;
	UPROPERTY(EditAnywhere)
	float CollisionAvoidanceWeight = 3.0f;

	UPROPERTY(EditAnywhere)
	AActor* Target;
	UPROPERTY(EditAnywhere)
	float TargetWeight = 0.5f;

	UPROPERTY(EditAnywhere)
	int32 NeighbourCount = 20;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* SkeletalMesh;

	
protected:
	TArray<AActor*> Neighbours;
	TArray<FVector> TraceDirections;

	TArray<AActor*> Boids;

	const float AngleIncrement = PI * 2.0f * UE_GOLDEN_RATIO;

private:
	bool IsAvoiding = false;
	bool IsSearching = true;
	float CollisionAvoidTime = 0.75f;
	FVector CollisionAvoidVector;
	FTimerHandle AvoidanceTimerHandle;
public:	
	// Sets default values for this actor's properties
	ABoid();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	FVector GetCohesionVector();
	FVector GetAlignmentVector();
	FVector GetSeperationVector();

	void InitTraceDirections(int TraceDirectionsCount);
	FVector GetAvoidVector();
	bool TraceForward(FHitResult& OutHit);

	void CheckNeighbourDistances();
	void CheckBoidsDistances();

	void StopAvoiding();

	void DrawDebug();
};
