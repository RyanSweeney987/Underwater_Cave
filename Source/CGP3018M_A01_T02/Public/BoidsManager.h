// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <CGP3018M_A01_T02/Public/Boid.h>

#include "BoidsManager.generated.h"

UCLASS()
class CGP3018M_A01_T02_API ABoidsManager : public AActor
{
	GENERATED_BODY()
	

public:
	UPROPERTY(VisibleAnywhere)
	int32 BoidCount = 0;
	UPROPERTY(EditAnywhere)
	int32 GroupCount = 50;
	UPROPERTY(EditAnywhere)
	UClass* BoidClass;
protected:
	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> Boids;
private:
	USceneComponent* Scene;
	int32 TotalSpawned = 0;
public:	
	// Sets default values for this actor's properties
	ABoidsManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
