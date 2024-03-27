// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroOverlappedVolume.h"
#include "CandyHeroComponent.h"
#include "Drone_Lyra.h"



AHeroOverlappedVolume::AHeroOverlappedVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	Spawner = CreateDefaultSubobject<USpawningEnemiesComponent>(TEXT("Spawner"));
	OnActorBeginOverlap.AddDynamic(this, &AHeroOverlappedVolume::OnOverlapBegin);
	bIsGivingOutAward = true;
	bIsTakingAward = false;
	bIsAttacking = false;

}

// Called when the game starts or when spawned
void AHeroOverlappedVolume::BeginPlay()
{
	Super::BeginPlay();
	//If the volume is not Taking award then the boolean IsAwardReceived is automatically set to true
	if (!bIsTakingAward)
	{
		IsAwardReceived = true;
		OnReceivedAwardDelegate.Broadcast();
	}
	Spawner->MaxActorSpawned = NumChocolate;
	RotatingMesh = FindComponentByClass<UStaticMeshComponent>();
}

// Called every frame
void AHeroOverlappedVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsActivated)
	{
		if (RotatingMesh)
		{
			FRotator NewRotation;
			NewRotation.Yaw = DeltaTime*30.f;
			RotatingMesh->AddWorldRotation(NewRotation);
		}
	}
}

void AHeroOverlappedVolume::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (bIsActivated)
	{
		if (bIsGivingOutAward)
		{
			Spawner->MaxActorSpawned = NumChocolate;
			SpawningAward(OtherActor);
		}

		if (bIsTakingAward)
		{
			TakingAward(OtherActor);
		}

		if (bIsAttacking &!IsAttackedGenerated)
		{
			GeneratingAttack(OtherActor);
		}
	}
}


void AHeroOverlappedVolume::SpawningAward(class AActor* OtherActor)
{
	if (UCandyHeroComponent* hero_comp = OtherActor->FindComponentByClass<class UCandyHeroComponent>())
	{
		HeroCompRef = hero_comp;
		if (!hero_comp->CheckOwnerHasAward())
		{
			APawn* HeroPawn = Cast<APawn>(hero_comp->GetOwner());
			Spawner->SpawnedActorType = hero_comp->AwardType;
			Spawner->SpawnActoratLocation(HeroPawn->GetActorLocation() + 100 * HeroPawn->GetActorRightVector(), HeroPawn->GetActorRotation());
			hero_comp->TakeAward(Spawner->SpawnedActor);
		}
	}

}





void AHeroOverlappedVolume::TakingAward(class AActor* OtherActor)
{
	if (UCandyHeroComponent* hero_comp = OtherActor->FindComponentByClass<class UCandyHeroComponent>())
	{
		HeroCompRef = hero_comp;
		//TODO: Clear Timer if End Overlap
		FTimerManager& TimerManager = GetWorldTimerManager();
		TimerManager.SetTimer(TimerHandle_Vol, this, &AHeroOverlappedVolume::AfterTimeUp, 5.0f, false);
	}
}

void AHeroOverlappedVolume::GeneratingAttack(class AActor* OtherActor)
{
	// only spawn when the actor is a drone (instead of other actors that could have the candyherocomponent)
	if (ADrone_Lyra* hero_drone = Cast<ADrone_Lyra>(OtherActor))
	{
		if (hero_drone->CandyHero->CheckOwnerHasAward())
		{
			const FActorSpawnParameters spawn_params;
			FVector new_location = hero_drone->GetActorLocation()+FMath::VRand()*300.f;
			GetWorld()->SpawnActor<AActor>(EnemyType, new_location,hero_drone->GetActorRotation(), spawn_params);
			IsAttackedGenerated = true;

		}
		
	}

}

void AHeroOverlappedVolume::AfterTimeUp()
{
	TSet<AActor*> overlapped_actors;
	GetOverlappingActors(overlapped_actors);
	if (HeroCompRef)
	{
		if (overlapped_actors.Contains(HeroCompRef->GetOwner()))
		{
			if (bIsTakingAward)
			{
				HeroCompRef->DestoryAward();
				IsAwardReceived = true;
				bIsActivated = false;
				OnReceivedAwardDelegate.Broadcast();
			}
		}
	}
}

bool AHeroOverlappedVolume::IsActivated()
{
	return bIsActivated;
}

void AHeroOverlappedVolume::SetbIsActivated(bool newval)
{
	bIsActivated = newval;
}

bool AHeroOverlappedVolume::GetIsAwardReceived()
{
	return IsAwardReceived;
}

