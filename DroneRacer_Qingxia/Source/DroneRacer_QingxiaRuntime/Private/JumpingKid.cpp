// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpingKid.h"
#include <AIModule/Classes/Perception/AIPerceptionComponent.h>
#include <AIModule/Classes/Perception/AISenseConfig_Sight.h>
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "Character/LyraCharacterMovementComponent.h"
#include "CandyHeroComponent.h"
#include "Drone_Lyra.h"
#include <AIModule/Classes/AISystem.h>


AJumpingKid::AJumpingKid(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	AIVision = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	VisionConfig= CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Vision Config"));
	
	//Set up AI Perception System
	VisionConfig->SightRadius = 500.0f;
	VisionConfig->LoseSightRadius = 1000.0f;
	VisionConfig->PeripheralVisionAngleDegrees = 360.0f;
	VisionConfig->DetectionByAffiliation.bDetectEnemies = true;
	VisionConfig->DetectionByAffiliation.bDetectFriendlies = true;
	VisionConfig->DetectionByAffiliation.bDetectNeutrals = true;
	VisionConfig->SetMaxAge(0.1f);

	AIVision->ConfigureSense(*VisionConfig);
	AIVision->SetDominantSense(VisionConfig->GetSenseImplementation());
	AIVision->OnPerceptionUpdated.AddDynamic(this, &AJumpingKid::PerceptionUpdated);
	AIVision->SetSenseEnabled(UAISense_Sight::StaticClass(), true);

	bStartChasing = false;
	bGoingBack = false;

	//Initialize Movement
	ULyraCharacterMovementComponent* LyraMoveComp = Cast<ULyraCharacterMovementComponent>(GetCharacterMovement());
	LyraMoveComp->GravityScale = 0.0f;
	LyraMoveComp->bOrientRotationToMovement = true;
}

bool AJumpingKid::GetbStartChasing()
{
	return bStartChasing;
}

void AJumpingKid::BeginPlay()
{
	Super::BeginPlay();
	OrigLoc = GetActorLocation();
}

void AJumpingKid::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	float DistanceTravelled = FMath::Abs((GetActorLocation() - OrigLoc).Size());
	bGoingBack = (DistanceTravelled >= MaxDistanceTravelled);

	if (bGoingBack)
	{
		GoBackToLocation();
	}
	else if (bStartChasing)
	{
		JumpToDrone();
	}
}

void AJumpingKid::PerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	if (UpdatedActors.IsEmpty())
	{
		Drone = NULL;
		bStartChasing = false;
	}
	else
	{
		for (AActor* DetectedActor : UpdatedActors)
		{
			if (DetectedActor && DetectedActor->IsA(ADrone_Lyra::StaticClass()))
			{
				Drone = Cast<APawn>(DetectedActor);
				bStartChasing = true;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Not A Drone"));
			}
		}
	}

	
}

void AJumpingKid::TakeAwardAway()
{
	if (Drone)
	{
		if (UCandyHeroComponent* HeroComp = Drone->FindComponentByClass<UCandyHeroComponent>())
		{
			if (HeroComp->CheckOwnerHasAward())
			{
				OnKidAttackStartDelegate.Broadcast();
				HeroComp->AwardTakenAway(this);
				bStartChasing = false;
				Drone = NULL;
			}
		}
	}
}

void AJumpingKid::GoBackToLocation()
{
	SetActorLocation(OrigLoc);
}

void AJumpingKid::OnVisionDeactivated(UActorComponent* Component)
{
	Drone = NULL;
	UE_LOG(LogTemp, Warning, TEXT("KIDVISIONDISABLED"));
}

void AJumpingKid::JumpToDrone()
{
	if (Drone)
	{
		FVector DroneLoc = Drone->GetActorLocation();
		FVector MyLoc = GetActorLocation();
		FVector MovementDirection = DroneLoc - MyLoc;
		MovementDirection.Normalize();
		AddMovementInput(MovementDirection, MovingSpeed);

		FVector NewPos = GetActorLocation() - Drone->GetActorLocation();
		if (NewPos.Size()<=MaxChocolateDistance)
		{
			TakeAwardAway();
		}
	}
}
