// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "DroneGameStateComponent.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMissionComplete);
class AHeroOverlappedVolume;
class  ULyraExperienceDefinition;
struct FLyraVerbMessage;


UCLASS()
class DRONERACER_QINGXIARUNTIME_API UDroneGameStateComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UDroneGameStateComponent(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(EditAnywhere)
	float AllowedDisownedTime = 10.f;

	UPROPERTY(EditAnywhere)
	class UPrimaryDataAsset* Customized_Data;

	UPROPERTY(BlueprintReadWrite)
	TArray<AHeroOverlappedVolume*> Checkpoints;

public:
	virtual void BeginPlay();
	void InitializingVariables();
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	UPROPERTY(BlueprintAssignable)
	FOnMissionComplete OnMissionCompleteDelegate;

	UFUNCTION(BlueprintCallable)
	void OnAwardReceived();

	UFUNCTION()
	void ActivateCheckPoints();

	void OnExperienceLoaded(const ULyraExperienceDefinition* experience_def);
private:
	bool bMissionCompleted = false;
	bool bSetUpDelegates = false;

	UFUNCTION()
		void RemoveCheckPoint(FGameplayTag InChannel, const FLyraVerbMessage& InMessage);
	UFUNCTION()
		void AddCheckPoint(FGameplayTag InChannel, const FLyraVerbMessage& InMessage);
	
};
