// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particle.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Flock.generated.h"

UCLASS()
class BOIDS_API AFlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY (EditAnywhere , category = "custom")
	UInstancedStaticMeshComponent* ISM;

	UPROPERTY(EditAnywhere, category = "custom")
	FVector Direction = FVector(0., 0., 0.);

	UPROPERTY(EditAnywhere, category = "custom")
	float DistanceMin = 250.;

	UPROPERTY(EditAnywhere, category = "custom")
	int NumofBoids = 1000;

	UPROPERTY(EditAnywhere, category = "custom")
	float Radius = 1000.;

	UPROPERTY(EditAnywhere , category = "custom")
	float ViewAngle = 120.f;

	UPROPERTY(EditAnywhere, category = "custom")
	TArray<FVector> passagepoints;

	FVector separate(int32 index);

	FVector align(int32 index);

	FVector cohesion(int32 index);

	void CreateInstances();

	void update();

	TArray<Particle*> particles;

	bool isinview(int32 i, int32 ii) const;

	void updestination();

	void GenerateGoldenSpherePoints(int32 NumPoints, TArray<FVector>& Points);

	TArray<class UInstancedStaticMeshComponent> ISMlist; 



};
