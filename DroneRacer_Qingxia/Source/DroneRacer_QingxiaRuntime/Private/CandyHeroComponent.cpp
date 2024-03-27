// Fill out your copyright notice in the Description page of Project Settings.


#include "CandyHeroComponent.h"
#include <GameFramework/Actor.h>
#include "GameFramework/GameplayMessageSubsystem.h"
#include <GameplayTagContainer.h>
#include "DroneGameStateComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Messages/LyraVerbMessage.h"

// Sets default values for this component's properties
UCandyHeroComponent::UCandyHeroComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UCandyHeroComponent::BeginPlay()
{
	Super::BeginPlay();
	//Initialize AwardList
	SetAwardVariables();

	//Listen for game start to start the timer
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	FGameplayTag ChannelTag = FGameplayTag::RequestGameplayTag(FName("InitState.GameplayReady"));
	FGameplayMessageListenerHandle ListenerHandle = MessageSubsystem.RegisterListener(ChannelTag, this, &UCandyHeroComponent::StartTimer);
	
}


// Called every frame
void UCandyHeroComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UCandyHeroComponent::CheckOwnerHasAward()
{
	return bOwnerHasAward;
}

void UCandyHeroComponent::SetAwardVariables()
{
	APawn* OwnerPawn = GetOwner<APawn>();
	if (OwnerPawn)
	{
		TArray<AActor*> ReturnedActors;
		OwnerPawn->GetAttachedActors(ReturnedActors);
		for (int idx = 0; idx < ReturnedActors.Num(); idx++)
		{
			if (ReturnedActors[idx]->IsA(AwardType))
			{
				bOwnerHasAward = true;
				AwardNum++;
				AwardListArray.Add(ReturnedActors[idx]);
			}
		}
	}

	UDroneGameStateComponent* DroneGameStateComponent = GetWorld()->GetGameState()->FindComponentByClass<UDroneGameStateComponent>();
	if (DroneGameStateComponent)
	{
		MaxAllowedTime = DroneGameStateComponent->AllowedDisownedTime;
	}
}

void UCandyHeroComponent::StartTimer(FGameplayTag InChannel, const FGameplayTag& InMessage)
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(ChocolateDisowningCounter,this, &UCandyHeroComponent::AddToTime,1.0f,true,1.f);
}

void UCandyHeroComponent::AddToTime()
{
	if (CheckOwnerHasAward())
	{
		DisownedChocoTime = 0.f;
	}
	else
	{
		if (DisownedChocoTime< MaxAllowedTime)
		{
			DisownedChocoTime++;
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(ChocolateDisowningCounter);
			DisownedChocoTime = 0.f;
			UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
			FGameplayTag ChannelTag = FGameplayTag::RequestGameplayTag(FName("GameplayEvent.Death"));
			FLyraVerbMessage InMessage{};
			MessageSubsystem.BroadcastMessage(ChannelTag, InMessage);
		}
	}
}

void UCandyHeroComponent::DestoryAward()
{
	if (!AwardListArray.IsEmpty())
	{
		AwardListArray.Pop()->Destroy();
	}

	if (AwardListArray.IsEmpty())
	{
		bOwnerHasAward = false;
	}

	OnActivateCheckPointDelegate.Broadcast();
}

void UCandyHeroComponent::AwardTakenAway(AActor* NewOwner)
{
	if (bOwnerHasAward)
	{
		AActor* LastChocolate = AwardListArray.Pop();
		LastChocolate->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		LastChocolate->AttachToActor(NewOwner, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		FTransform NewTransform;
		NewTransform.SetTranslation(FVector(0, 100.f, 0));
		LastChocolate->SetActorRelativeTransform(NewTransform);
	}

	if (AwardListArray.IsEmpty())
	{
		bOwnerHasAward = false;
	}

	OnActivateCheckPointDelegate.Broadcast();
}

void UCandyHeroComponent::TakeAward(AActor* Award)
{
	if (Award->IsA(AwardType))
	{
		AwardListArray.Add(Award);
		Award->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
	}

	if (!AwardListArray.IsEmpty())
	{
		bOwnerHasAward = true;
	}

	OnActivateCheckPointDelegate.Broadcast();
}

