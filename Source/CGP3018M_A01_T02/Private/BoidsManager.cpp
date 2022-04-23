// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidsManager.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Sets default values
ABoidsManager::ABoidsManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
}

// Called when the game starts or when spawned
void ABoidsManager::BeginPlay()
{
	Super::BeginPlay();
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BoidClass, Boids);
	BoidCount = Boids.Num();
}

// Called every frame
void ABoidsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TotalSpawned < BoidCount) {
		int amountToSpawn = GroupCount;
		if (TotalSpawned + amountToSpawn > BoidCount) {
			amountToSpawn = BoidCount - TotalSpawned;
		}

		for (int i = TotalSpawned; i < TotalSpawned + amountToSpawn; i++) {
			Boids[i]->SetActorTickEnabled(true);
		}

		TotalSpawned += amountToSpawn;
	}
}

