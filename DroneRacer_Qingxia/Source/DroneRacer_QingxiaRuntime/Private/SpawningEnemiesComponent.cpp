// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawningEnemiesComponent.h"


// Sets default values for this component's properties
USpawningEnemiesComponent::USpawningEnemiesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USpawningEnemiesComponent::SpawnActoratLocation(const FVector& ActorLocation, const FRotator& ActorRotation)
{
	if (NumActorSpawned < MaxActorSpawned)
	{
		const FActorSpawnParameters SpawnParams;
		SpawnedActor = GetWorld()->SpawnActor<AActor>(SpawnedActorType, ActorLocation, ActorRotation, SpawnParams);
		NumActorSpawned++;
	}
}



void USpawningEnemiesComponent::SetupActorAttachement(AActor* NewParent)
{
	if (SpawnedActor)
	{
		SpawnedActor->AttachToActor(NewParent, FAttachmentTransformRules::KeepWorldTransform);
		SpawnedActor = nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Actor is spawned, hence cannot set up attachment"));
	}
}

// Called when the game starts
void USpawningEnemiesComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void USpawningEnemiesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

