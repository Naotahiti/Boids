// Fill out your copyright notice in the Description page of Project Settings.

#include "Flock.h"
#include "Particle.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AFlock::AFlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BoidsMesh"));

}

// Called when the game starts or when spawned
void AFlock::BeginPlay()
{
	Super::BeginPlay();

	FVector AL = GetActorLocation();
	for(FVector v : passagepoints)
	{
		v = v + AL;
	}

	CreateInstances();
	
}

// Called every frame
void AFlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
		for (int i = 0; i < particles.Num(); i++)
		{
			
				//Direction = separate(i);
			
			FVector Acceleration = Direction +separate(i)+align(i) +cohesion(i);

		

				// particles[i]->Velocity = Acceleration * DeltaTime;
				//particles[i]->Velocity = particles[i]->Velocity.GetClampedToMaxSize(1000.);
				//particles[i]->Position += particles[i]->Velocity * DeltaTime ;
				particles[i]->Position += Acceleration * DeltaTime * 100.;
			
				
			//particles[i]->Update(DeltaTime, Direction);
			
			ISM->UpdateInstanceTransform(i, FTransform(particles[i]->Position), false, true);

		}
	
	ISM->MarkRenderStateDirty();
	
}

bool AFlock::isinview(int32 i, int32 ii) const
{
	const FVector BoidPos = particles[i]->Position;
	const FVector BoidForward = particles[i]->Velocity.GetSafeNormal();

	const FVector ToOther =
		(particles[ii]->Position - BoidPos).GetSafeNormal();

	// Sécurité
	if (BoidForward.IsNearlyZero())
		return false;

	float Dot = FVector::DotProduct(BoidForward, ToOther);

	// Clamp pour éviter NaN avec Acos
	Dot = FMath::Clamp(Dot, -1.f, 1.f);

	float Angle = FMath::Acos(Dot);

	return Angle < FMath::DegreesToRadians(ViewAngle);

	//return true;
}

void AFlock::updestination()
{

}

void AFlock::GenerateGoldenSpherePoints(int32 NumPoints, TArray<FVector>& Points)
{
}

// create particles one by one
void AFlock::CreateInstances()
{
	for (int i = 0; i < NumofBoids; i++)

	{

		FVector RandomPos = FMath::VRand() * Radius;
		//FVector RandomVel = FMath::VRand().GetSafeNormal();
		FVector RandomVel = FMath::VRand().GetSafeNormal() * 100.;


		Particle* NewBoid = new Particle(RandomPos, Direction);
		particles.Add(NewBoid);

		// add new mesh each time a boid is created
		FTransform InstanceTransform;
		InstanceTransform.SetLocation(RandomPos);
		ISM->AddInstance(InstanceTransform);

	}
}


FVector AFlock::separate(int32 index)
{
	FVector Force = FVector::ZeroVector;

	for (int j = 0; j < particles.Num(); j++)
	{
		if (index == j) continue;

		float Dist = FVector::Dist(
			particles[index]->Position,
			particles[j]->Position
		);

		if (Dist > 0 && Dist < DistanceMin && isinview(index,j))
		{
			Force = (Force + (particles[index]->Position - particles[j]->Position).GetSafeNormal()).GetSafeNormal();
		
		}
		
	}

	return Force;

	/*Ratio = 1 - Length(MyBoid.Position - OtherBoid.Position) / MaxDistance
		OtherDirection = Normalize(MyBoid.Position - OtherBoid.Position)
		Direction = Normalize(Direction + OtherDirection * Ratio)*/

}

FVector AFlock::align(int32 index)
{
	FVector AvgVel = FVector::ZeroVector;
	int Count = 0;

	for (int j = 0; j < particles.Num(); j++)
	{
		if (index == j) continue;

		float Dist = FVector::Dist(particles[index]->Position, particles[j]->Position);
			

		if (Dist < 500. )//&& isinview(index, j))
		{
			AvgVel += particles[j]->Velocity;
			Count++;
		}
	}

	if (Count > 0)
	{
		AvgVel /= Count;
		return AvgVel.GetSafeNormal();
	}

	return  FVector::ZeroVector;
}

FVector AFlock::cohesion(int32 index)
{

	//Centroid.X = Sum(AllBoidsInRange.X) / NumBoidsInRange
	//	Centroid.Y = Sum(AllBoidsInRange.Y) / NumBoidsInRange

	FVector Sum = FVector::ZeroVector;
	int32 NumBoidsInRange = 0;

	const FVector MyPos = particles[index]->Position;

	for (int j = 0; j < particles.Num(); j++)
	{
		if (j == index) continue;

		const FVector OtherPos = particles[j]->Position;
		const float Dist = FVector::Dist(MyPos, OtherPos);

		if (Dist > DistanceMin )//&& isinview(index, j))
		{
			Sum += OtherPos;
			NumBoidsInRange++;
		}
		else return FVector::ZeroVector;
	}

	if (NumBoidsInRange == 0)
	{
		return FVector::ZeroVector;
	}

	FVector Centroid;
	Centroid.X = Sum.X / particles.Num();
	Centroid.Y = Sum.Y / particles.Num();
	Centroid.Z = Sum.Z / particles.Num();

	// Direction vers le centre du groupe
	return (Centroid - MyPos).GetSafeNormal();
	
}

