// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include <../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h>
#include "Character/LyraPawnExtensionComponent.h"
#include <Components/GameFrameworkInitStateInterface.h>
#include "Drone_Lyra.generated.h"

class UStaticMeshComponent;
class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeUp);
class ULyraAbilitySystemComponent;
UCLASS()
class DRONERACER_QINGXIARUNTIME_API ADrone_Lyra : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADrone_Lyra(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere)
	UCandyHeroComponent* CandyHero;

	UPROPERTY(EditAnywhere)
	UFloatingPawnMovement* FloatingMove;

	UPROPERTY(BlueprintReadOnly)
	ULyraPawnExtensionComponent* PawnExpCom;

	UPROPERTY(BlueprintAssignable)
	FOnTimeUp OnTimeUpDelegate;

	
	//Exposing Maximum Allowed Time in the Editor 
	UPROPERTY(EditAnywhere)
	float MaxTimeOverWrite = 0.f;
	bool bStartflying = false;
	float ChocoDisownedTime = 0.f;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Input_Move(const FInputActionValue& InputActionValue);

	void Input_LookMouse(const FInputActionValue& InputActionValue);

	void Input_Fly(const FInputActionValue& InputActionValue);
	void Input_Stop(const FInputActionValue& InputActionValue);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);

	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	UFUNCTION(BlueprintCallable)
	class UCandyHeroComponent* GetCandyHeroComponent();

	UFUNCTION(BlueprintCallable)
	bool GetIsFlying();

	UFUNCTION(BlueprintCallable)
	float GetChocoDisownedTime();
private:
	UStaticMeshComponent* FindHeroDrone(const FName TagName = FName("HeroDrone"));
	float LocalDeceleration;
};
