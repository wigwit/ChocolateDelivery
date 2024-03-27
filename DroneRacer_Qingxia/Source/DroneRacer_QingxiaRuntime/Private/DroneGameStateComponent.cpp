// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneGameStateComponent.h"
#include <Kismet/GameplayStatics.h>
#include "HeroOverlappedVolume.h"
#include "Drone_Lyra.h"
#include "CandyHeroComponent.h"
#include "GameModes/LyraExperienceManagerComponent.h"
#include "GameModes/LyraExperienceDefinition.h"
#include "Messages/LyraVerbMessage.h"
#include "GameFramework/GameplayMessageSubsystem.h"

UDroneGameStateComponent::UDroneGameStateComponent(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDroneGameStateComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ULyraExperienceManagerComponent* ExperienceComponent = GetOwner()->FindComponentByClass<ULyraExperienceManagerComponent>();
	if (IsValid(ExperienceComponent)) {
		ExperienceComponent->CallOrRegister_OnExperienceLoaded_LowPriority(FOnLyraExperienceLoaded::FDelegate::CreateUObject(this, &UDroneGameStateComponent::OnExperienceLoaded));
	}
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	FGameplayTag ChannelTagAdd = FGameplayTag::RequestGameplayTag(FName("GameplayEvent.AddCheckPoint"));
	FGameplayMessageListenerHandle ListenerHandle = MessageSubsystem.RegisterListener(ChannelTagAdd, this, &UDroneGameStateComponent::AddCheckPoint);

	FGameplayTag ChannelTagRemove = FGameplayTag::RequestGameplayTag(FName("GameplayEvent.RemoveCheckPoint"));
	FGameplayMessageListenerHandle ListenerHandle2 = MessageSubsystem.RegisterListener(ChannelTagAdd, this, &UDroneGameStateComponent::RemoveCheckPoint);
}

void UDroneGameStateComponent::InitializingVariables()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHeroOverlappedVolume::StaticClass(), FoundActors);

	APawn* drone = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	ADrone_Lyra* drone_lyra = Cast<ADrone_Lyra>(drone);
	UCandyHeroComponent* HeroCompRef = drone_lyra->CandyHero;
	HeroCompRef->OnActivateCheckPointDelegate.AddDynamic(this, &UDroneGameStateComponent::ActivateCheckPoints);
	for (AActor* actor : FoundActors)
	{
		AHeroOverlappedVolume* checkpoint = Cast<AHeroOverlappedVolume>(actor);
		Checkpoints.Add(checkpoint);
		if (checkpoint->bIsGivingOutAward)
		{
			checkpoint->SetbIsActivated(true);
		}
		else
		{
			checkpoint->SetbIsActivated(false);
		}
		checkpoint->OnReceivedAwardDelegate.AddDynamic(this, &UDroneGameStateComponent::OnAwardReceived);
	}

}

void UDroneGameStateComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDroneGameStateComponent::OnAwardReceived()
{

	for (auto i:Checkpoints)
	{
		if (!i->GetIsAwardReceived())
		{
			bMissionCompleted = false;
			break;
		}
		else
		{
			bMissionCompleted = true;
		}

	}

	if (bMissionCompleted)
	{
		OnMissionCompleteDelegate.Broadcast();
	}
}

void UDroneGameStateComponent::ActivateCheckPoints()
{
	APawn* DroneRef = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	ADrone_Lyra* DroneLyraRef = Cast<ADrone_Lyra>(DroneRef);
	UCandyHeroComponent* HeroCompRef = DroneLyraRef->CandyHero;
	if (HeroCompRef->CheckOwnerHasAward())
	{
		for (auto i:Checkpoints)
		{
			if (i)
			{
				if (!i->GetIsAwardReceived())
				{
					i->SetbIsActivated(true);
				}
				else
				{
					i->SetbIsActivated(false);
				}
			}
		}
	}
	else
	{
		for (auto i : Checkpoints)
		{
			if (i)
			{
				if (i->bIsGivingOutAward)
				{
					i->SetbIsActivated(true);
				}
				else
				{
					i->SetbIsActivated(false);
				}
			}
		}
	}
}

void UDroneGameStateComponent::OnExperienceLoaded(const ULyraExperienceDefinition* experience_def)
{
	FString ExperienceName = *experience_def->GetName();
	if (ExperienceName.Contains(FString("Debug")))
	{
		this->Deactivate();
	}
	else
	{
		InitializingVariables();
	}
}

void UDroneGameStateComponent::RemoveCheckPoint(FGameplayTag InChannel, const FLyraVerbMessage& InMessage)
{
	if (AHeroOverlappedVolume* Checkpoint = Cast<AHeroOverlappedVolume>(InMessage.Instigator))
	{
		Checkpoints.Empty();
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHeroOverlappedVolume::StaticClass(), FoundActors);
		for (AActor* actor : FoundActors)
		{
			AHeroOverlappedVolume* checkpoint = Cast<AHeroOverlappedVolume>(actor);
			Checkpoints.Add(checkpoint);
		}
		ActivateCheckPoints();
	}

}

void UDroneGameStateComponent::AddCheckPoint(FGameplayTag InChannel, const FLyraVerbMessage& InMessage)
{
	if (AHeroOverlappedVolume* Checkpoint = Cast<AHeroOverlappedVolume>(InMessage.Instigator))
	{
		Checkpoints.Add(Checkpoint);
		ActivateCheckPoints();
	}
}
