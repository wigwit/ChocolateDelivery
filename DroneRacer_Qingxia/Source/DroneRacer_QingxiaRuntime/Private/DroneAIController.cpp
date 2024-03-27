// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneAIController.h"
#include <AIModule/Classes/Perception/AIPerceptionComponent.h>
#include <AIModule/Classes/Perception/AISenseConfig_Sight.h>
#include "JumpingKid.h"

ADroneAIController::ADroneAIController(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	AIVision = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	VisionConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Vision Config"));

	VisionConfig->SightRadius = 500.0f;
	VisionConfig->LoseSightRadius = 1000.0f;
	VisionConfig->PeripheralVisionAngleDegrees = 360.0f; 
	VisionConfig->DetectionByAffiliation.bDetectEnemies = true;
	VisionConfig->DetectionByAffiliation.bDetectFriendlies = true;
	VisionConfig->DetectionByAffiliation.bDetectNeutrals = true;
	VisionConfig->SetMaxAge(0.1f);

	AIVision->ConfigureSense(*VisionConfig);
	AIVision->SetDominantSense(VisionConfig->GetSenseImplementation());
	AIVision->OnPerceptionUpdated.AddDynamic(this,&ADroneAIController::PerceptionUpdated);
	AIVision->OnTargetPerceptionForgotten.AddDynamic(this, &ADroneAIController::WhenPerceptionForget);
}

void ADroneAIController::PerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	if (UpdatedActors.IsEmpty())
	{
		TargetActor = NULL;
	}
	else 
	{
		for (AActor* DetectedActor : UpdatedActors)
		{
			if (DetectedActor&&DetectedActor->IsA(TargetActorType))
			{
				TargetActor = DetectedActor;
			}
		}
	}
}

void ADroneAIController::WhenPerceptionForget(AActor* Actor)
{
	TargetActor = NULL;
}