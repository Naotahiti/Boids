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



	CreateInstances();
	
}

// Called every frame
void AFlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
		for (int i = 0; i < particles.Num(); i++)
		{
			
				//Direction = separate(i);
			
			FVector Acceleration = Direction +
				separate(i);//+ * SeparationWeight +
					//align(i) +//* AlignmentWeight +
					//cohesion(i);// *CohesionWeight;

			/*	 particles[i]->Velocity += Acceleration * DeltaTime;
				particles[i]->Velocity = particles[i]->Velocity.GetClampedToMaxSize(100.);*/
				particles[i]->Position += particles[i]->Velocity * DeltaTime * 100.;
				particles[i]->Position += Acceleration * DeltaTime * 100.;
			
			//particles[i]->Update(DeltaTime, Direction);
			//FTransform T(FRotator::ZeroRotator, particles[i]->Position);
			ISM->UpdateInstanceTransform(i, FTransform(particles[i]->Position), false, true);

		}
	
	ISM->MarkRenderStateDirty();
	
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

		if (Dist > 0 && Dist < 500.)
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
			

		if (Dist < 500.)
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

	FVector Center = FVector::ZeroVector;
	int Count = 0;

	for (int j = 0; j < particles.Num(); j++)
	{
		if (index == j) continue;

		float Dist = FVector::Dist(particles[index]->Position, particles[j]->Position);

		if (Dist < 500.)
		{
			Center += particles[j]->Position;
			Count++;
		}
	}

	if (Count > 0)
	{
		Center /= Count;
		return (Center - particles[index]->Position).GetSafeNormal();
	}

	return  FVector::ZeroVector;
	
}

