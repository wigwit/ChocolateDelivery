// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CandyHeroComponent.generated.h"

class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivateCheckPoint);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DRONERACER_QINGXIARUNTIME_API UCandyHeroComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCandyHeroComponent();
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> AwardType;
	 
	UPROPERTY(BlueprintReadOnly)
	bool bOwnerHasAward = false;

	UPROPERTY(BlueprintReadOnly)
	int AwardNum = 0;

	UPROPERTY(BlueprintAssignable)
	FOnActivateCheckPoint OnActivateCheckPointDelegate;

private:
	TArray<AActor*> AwardListArray;
	void SetAwardVariables();

	UFUNCTION(BlueprintCallable)
	void StartTimer(FGameplayTag InChannel, const FGameplayTag& InMessage);

	UPROPERTY()
	FTimerHandle ChocolateDisowningCounter;

	UFUNCTION()
	void AddToTime();

	float DisownedChocoTime=0.f;

	float MaxAllowedTime;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	bool CheckOwnerHasAward();

	UFUNCTION(BlueprintCallable)
	void DestoryAward();

	UFUNCTION(BlueprintCallable)
	void AwardTakenAway(AActor* newOwner);

	UFUNCTION(BlueprintCallable)
	void TakeAward(AActor* award);



		
};
