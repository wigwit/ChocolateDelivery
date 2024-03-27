// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "SpawningEnemiesComponent.h"
#include <Engine/TimerHandle.h>
#include <UObject/Object.h>
#include "JumpingKid.h"
#include "HeroOverlappedVolume.generated.h"

class APawn;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReceivedAward);
UCLASS()
class DRONERACER_QINGXIARUNTIME_API AHeroOverlappedVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHeroOverlappedVolume();
	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(BlueprintReadOnly)
	USpawningEnemiesComponent* Spawner;

	class UCandyHeroComponent* HeroCompRef; 

	UPROPERTY(BlueprintAssignable)
	FOnReceivedAward OnReceivedAwardDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsGivingOutAward;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bIsTakingAward;


	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bIsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int NumChocolate=5;

	UPROPERTY()
	FTimerHandle TimerHandle_Vol;

	UPROPERTY(EditAnywhere)
	bool bIsActivated=false;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> EnemyType = AJumpingKid::StaticClass();

	bool IsAwardReceived = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
	bool IsActivated();

	UFUNCTION(BlueprintCallable)
		void SetbIsActivated(bool NewVal);

	UFUNCTION(BlueprintCallable)
		bool GetIsAwardReceived();

	UFUNCTION(BlueprintCallable)
		void AwardAutoReceived(bool NewVal){IsAwardReceived = NewVal; };

private:
	//Functionality that corresponds to different types of volume
	void SpawningAward(class AActor* OtherActor);
	void TakingAward(class AActor* OtherActor);
	void GeneratingAttack(class AActor* OtherActor);
	void AfterTimeUp();
	bool IsAttackedGenerated = false;
	UStaticMeshComponent* RotatingMesh;
};
