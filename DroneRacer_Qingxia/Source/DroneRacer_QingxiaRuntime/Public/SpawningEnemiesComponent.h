// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpawningEnemiesComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DRONERACER_QINGXIARUNTIME_API USpawningEnemiesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpawningEnemiesComponent();

	UFUNCTION(BlueprintCallable)
		void SpawnActoratLocation(const FVector& ActorLocation, const FRotator& ActorRotation);
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> SpawnedActorType;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		int MaxActorSpawned = 3;
	UPROPERTY(BlueprintReadWrite)
		AActor* SpawnedActor = nullptr;
	UFUNCTION(BlueprintCallable)
		void SetupActorAttachement(AActor* NewParent);
	UPROPERTY(BlueprintReadOnly)
		int NumActorSpawned=0;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
