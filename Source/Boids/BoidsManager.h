// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Flock.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "BoidsManager.generated.h"

UCLASS()
class BOIDS_API ABoidsManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoidsManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnParticles(); // par chunk

	TArray<int> chunks;

	UPROPERTY(EditAnywhere , Category = "boids")
	UInstancedStaticMeshComponent* ism;

	UPROPERTY(EditAnywhere , Category = "boids")
	TSubclassOf<AFlock> BoidsClass;





};
