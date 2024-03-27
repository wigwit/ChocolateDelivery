// Fill out your copyright notice in the Description page of Project Settings.S
#include "Drone_Lyra.h"
#include <GameFramework/PlayerController.h>
#include "Player/LyraLocalPlayer.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/LyraInputComponent.h"
#include "Character/LyraPawnData.h"

#include "Input/LyraInputConfig.h"
#include "LyraGameplayTags.h"
#include <Components/SceneComponent.h>
#include <Components/GameFrameworkComponentManager.h>
#include "DroneGameStateComponent.h"
#include <Kismet/GameplayStatics.h>
#include "CandyHeroComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Player/LyraPlayerState.h"
#include <AIModule/Classes/AISystem.h>


class ALyraPlayerState;
// Sets default values
ADrone_Lyra::ADrone_Lyra(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CandyHero = CreateDefaultSubobject<UCandyHeroComponent>(TEXT("CandyHero"));
	FloatingMove = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingMove"));
	PawnExpCom = CreateDefaultSubobject<ULyraPawnExtensionComponent>(TEXT("PawnExComp"));

	// Setting the values for movement component, but can be overwritten in the Editor
	FloatingMove->MaxSpeed = 700.0f;
	FloatingMove->Acceleration = 200.f;
	FloatingMove->Deceleration = 100.f;
}

// Called when the game starts or when spawned
void ADrone_Lyra::BeginPlay()
{
	Super::BeginPlay();

	// Accessing Maximum Allowed Time from Game State Component
	AGameStateBase* CurGameState = UGameplayStatics::GetGameState(this);
	if (UDroneGameStateComponent* TimeComp = CurGameState->FindComponentByClass<UDroneGameStateComponent>())
	{
		MaxTimeOverWrite = TimeComp->AllowedDisownedTime;
	}

	LocalDeceleration = FloatingMove->Deceleration;
}

// Called every frame
void ADrone_Lyra::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bStartflying)
	{
		FloatingMove->Velocity = GetActorForwardVector() * FloatingMove->Velocity.Size();
		if (FloatingMove->Velocity.IsNearlyZero(1e-2))
		{
			bStartflying = false;
		}
	}
}

// Called to bind functionality to input
void ADrone_Lyra::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	const APlayerController* PlayerController = GetController<APlayerController>();
	check(PlayerController);

	const ULyraLocalPlayer* LocalPlayer = Cast<ULyraLocalPlayer>(PlayerController->GetLocalPlayer());
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	ALyraPlayerState* LyraPS = GetPlayerState<ALyraPlayerState>();
	check(LyraPS);
	PawnExpCom->InitializeAbilitySystem(LyraPS->GetLyraAbilitySystemComponent(), LyraPS);

	// Binding PlayerInputComponent Using Lyra's System
	if (const ULyraPawnExtensionComponent* ExtCompTemp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(this))
	{
		if (const ULyraPawnData* PawnData = ExtCompTemp->GetPawnData<ULyraPawnData>())
		{
			if (const ULyraInputConfig* InputConfig = PawnData->InputConfig)
			{
				ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(PlayerInputComponent);
				if (ensureMsgf(LyraIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to ULyraInputComponent or a subclass of it.")))
				{
					// Add the key mappings that may have been set by the player
					LyraIC->AddInputMappings(InputConfig, Subsystem);

					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					// be triggered directly by these input actions Triggered events. 

					//NOTE: I have created a tag called InputTagFly from editor, which is stored in plugingameplaytags.ini under Lyra's config
					const FGameplayTag fly = LyraGameplayTags::FindTagByString("InputTagFly");
					const FGameplayTag stop = LyraGameplayTags::FindTagByString("InputTagStop");

					TArray<uint32> BindHandles;
					LyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		
					LyraIC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
					LyraIC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
					LyraIC->BindNativeAction(InputConfig, fly, ETriggerEvent::Triggered, this, &ThisClass::Input_Fly, /*bLogIfNotFound=*/ false);
					LyraIC->BindNativeAction(InputConfig, stop, ETriggerEvent::Triggered, this, &ThisClass::Input_Stop,/*bLogIfNotFound=*/ false);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Cannot Find Pawn Input Data, and Input Action Cannot Bind Correctly"));
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PlayerController), "BindInputsNow");
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, "BindInputsNow");
}

// WASD Controlled Move Logic
void ADrone_Lyra::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	if (Value.X != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(GetActorRightVector());
		AddMovementInput(MovementDirection, Value.X*0.5);

	}

	if (Value.Y != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(GetActorForwardVector());

		AddMovementInput(MovementDirection, Value.Y*0.5);
		
	}

}

// Mouse Controlled Move Logic
void ADrone_Lyra::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();
	const float y =Controller->GetControlRotation().Yaw;
	const float p = Controller->GetControlRotation().Pitch;
	if (Value.X != 0.0f )
	{
		AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f )
	{
		AddControllerPitchInput(Value.Y);
	}
}

// Accelerate and Decelerate Logic
void ADrone_Lyra::Input_Fly(const FInputActionValue& InputActionValue)
{
	const float acc_input = InputActionValue.Get<float>();
	if (acc_input > 0.0f)
	{
		// Manually Set Deceleration to 0 so that the drone will not decelerate to 0 unless told to 
		FloatingMove->Deceleration = 0.f;
		if (bStartflying == false)
		{
			if (FloatingMove->Velocity.Size()==0.f)
			{
				FloatingMove->Velocity = GetActorForwardVector()*150.f;
			}
			
			bStartflying = true;
		}
		else
		{
			if (FloatingMove->Velocity.Size()<FloatingMove->GetMaxSpeed())
			{
				FloatingMove->Velocity += FloatingMove->Acceleration * FloatingMove->Velocity.GetSafeNormal();
			}
		}	
	}
	else
	{
		FloatingMove->Deceleration = LocalDeceleration;
		FloatingMove->Velocity -= FloatingMove->Deceleration * FloatingMove->Velocity.GetSafeNormal();

	}
	

}

// Stop Movement
void ADrone_Lyra::Input_Stop(const FInputActionValue& InputActionValue)
{
	FloatingMove->Velocity = FVector(0.f, 0.f, 0.f);
	bStartflying = false;

}

void ADrone_Lyra::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	PawnExpCom->GetLyraAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
}

void ADrone_Lyra::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	PawnExpCom->GetLyraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
}

class UCandyHeroComponent* ADrone_Lyra::GetCandyHeroComponent()
{
	return CandyHero;
}

bool ADrone_Lyra::GetIsFlying()
{
	return bStartflying;

}

float ADrone_Lyra::GetChocoDisownedTime()
{
	return ChocoDisownedTime;
}

UStaticMeshComponent* ADrone_Lyra::FindHeroDrone(const FName TagName /*= FName("HeroDrone")*/)
{
	TArray<UActorComponent*> FoundComponents = GetComponentsByTag(UStaticMeshComponent::StaticClass(),TagName);
	UActorComponent* DroneModelRef = FoundComponents[0];
	UStaticMeshComponent* HeroDroneRef = Cast<UStaticMeshComponent>(DroneModelRef);
	check(HeroDroneRef);
	return HeroDroneRef;
}

