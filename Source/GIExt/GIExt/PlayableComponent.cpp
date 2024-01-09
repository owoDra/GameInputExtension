// Copyright (C) 2024 owoDra

#include "PlayableComponent.h"

#include "GameplayTag/GIETags_Input.h"
#include "InputConfig.h"
#include "GIExtLogs.h"

#include "InitState/InitStateTags.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayableComponent)


const FName UPlayableComponent::NAME_BindInputsNow("BindInputsNow");
const FName UPlayableComponent::NAME_ActorFeatureName("Playable");

UPlayableComponent::UPlayableComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UPlayableComponent::BeginPlay()
{
	ListenControllerChange();

	Super::BeginPlay();
}

void UPlayableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnlistenControllerChange();

	if (auto* Pawn{ GetPawn<APawn>() })
	{
		if (auto* Controller{ Pawn->GetController() })
		{
			UninitializePlayerInput(Controller);
		}
	}

	Super::EndPlay(EndPlayReason);
}


void UPlayableComponent::InitializePlayerInput(AController* Controller)
{
	if (auto* PC{ Cast<APlayerController>(Controller) })
	{
		// Check essential values

		check(DefaultInputConfig);
		
		if (auto* IC{ Cast<UEnhancedInputComponent>(PC->InputComponent) })
		{
			TArray<uint32> BindHandles;
			DefaultInputConfig->BindTagActions(IC, this, &ThisClass::TagInput_Pressed, &ThisClass::TagInput_Released, /*out*/ BindHandles);

			DefaultInputConfig->BindNativeAction(IC, TAG_Input_MouseAndKeyboard_Move, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_Move);
			DefaultInputConfig->BindNativeAction(IC, TAG_Input_Gamepad_Move, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_Move);
			DefaultInputConfig->BindNativeAction(IC, TAG_Input_MouseAndKeyboard_Look, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_LookMouse);
			DefaultInputConfig->BindNativeAction(IC, TAG_Input_Gamepad_Look, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_LookStick);
		}

		if (auto MappingContext{ DefaultInputConfig->DefaultMappingContext })
		{
			const auto* LP{ PC->GetLocalPlayer() };
			if (auto* Subsystem{ LP ? LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr })
			{
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(MappingContext, 0);
			}
		}

		UE_LOG(LogGIE, Log, TEXT("Playable Component Initialized"));

		UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(PC, NAME_BindInputsNow);
		UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(GetPawnChecked<APawn>(), NAME_BindInputsNow);
	}
}

void UPlayableComponent::UninitializePlayerInput(AController* Controller)
{
	if (auto* PC{ Cast<APlayerController>(Controller) })
	{
		const auto* Pawn{ GetPawnChecked<APawn>() };
		if (auto* IC{ Cast<UEnhancedInputComponent>(Pawn->InputComponent) })
		{
			IC->ClearBindingsForObject(this);
		}

		const auto* LP{ PC->GetLocalPlayer() };
		if (auto* Subsystem{ LP ? LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr })
		{
			Subsystem->ClearAllMappings();
		}

		UE_LOG(LogGIE, Log, TEXT("Playable Component Uninitialized"));
	}
}

void UPlayableComponent::ListenControllerChange()
{
	auto* Pawn{ GetPawnChecked<APawn>() };

	//Pawn->ReceiveControllerChangedDelegate.AddDynamic(this, &ThisClass::HandleControllerChanged);

	FScriptDelegate NewDelegate;
	NewDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UPlayableComponent, HandleControllerChanged));

	Pawn->ReceiveControllerChangedDelegate.Add(NewDelegate);
}

void UPlayableComponent::UnlistenControllerChange()
{
	auto* Pawn{ GetPawnChecked<APawn>() };

	//Pawn->ReceiveControllerChangedDelegate.RemoveDynamic(this, &ThisClass::HandleControllerChanged);

	Pawn->ReceiveControllerChangedDelegate.RemoveAll(this);
}

void UPlayableComponent::HandleControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	UninitializePlayerInput(OldController);
	InitializePlayerInput(NewController);
}


bool UPlayableComponent::CanChangeInitStateToDataInitialized(UGameFrameworkComponentManager* Manager) const
{
	return (DefaultInputConfig != nullptr);
}

void UPlayableComponent::HandleChangeInitStateToDataInitialized(UGameFrameworkComponentManager* Manager)
{
	if (auto* Pawn{ GetPawnChecked<APawn>() })
	{
		InitializePlayerInput(Pawn->GetController());
	}
}


void UPlayableComponent::SetInputConfig(const UInputConfig* NewInputConfig)
{
	if (auto* Pawn{ GetPawn<APawn>() })
	{
		if (Pawn->IsLocallyControlled())
		{
			if (DefaultInputConfig != NewInputConfig)
			{
				DefaultInputConfig = NewInputConfig;

				CheckDefaultInitialization();
			}
		}
	}
}

void UPlayableComponent::AddAdditionalInputConfig(const UInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	// Check essential values

	const auto* Pawn{ GetPawn<APawn>() };
	if (!Pawn)
	{
		return;
	}

	auto* IC{ Cast<UEnhancedInputComponent>(Pawn->InputComponent) };
	check(IC);

	const auto* PC{ GetController<APlayerController>() };
	check(PC);

	const auto* LP{ PC->GetLocalPlayer() };
	check(LP);

	auto* Subsystem{ LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() };
	check(Subsystem);

	// Bind input actions

	InputConfig->BindTagActions(IC, this, &ThisClass::TagInput_Pressed, &ThisClass::TagInput_Released, /*out*/ BindHandles);
}

void UPlayableComponent::RemoveAdditionalInputConfig(const UInputConfig* InputConfig)
{
	// @TODO
}


void UPlayableComponent::TagInput_Pressed(FGameplayTag InputTag)
{
	// UE_LOG(LogGIE, Log, TEXT("Recieved Tag Pressed Input(%s)"), *InputTag.GetTagName().ToString());

	TagInput_PressedExtra(InputTag);
}

void UPlayableComponent::TagInput_Released(FGameplayTag InputTag)
{
	// UE_LOG(LogGIE, Log, TEXT("Recieved Tag Released Input(%s)"), *InputTag.GetTagName().ToString());

	TagInput_ReleasedExtra(InputTag);
}

void UPlayableComponent::TagInput_PressedExtra(FGameplayTag InputTag)
{
}

void UPlayableComponent::TagInput_ReleasedExtra(FGameplayTag InputTag)
{
}

void UPlayableComponent::NativeInput_Move(const FInputActionValue& InputActionValue)
{
	auto* Pawn{ GetPawn<APawn>() };
	auto* Controller{ Pawn ? Pawn->GetController() : nullptr };

	if (Controller)
	{
		const auto Value{ InputActionValue.Get<FVector2D>() };
		const auto MovementRotation{ FRotator(0.0f, Controller->GetControlRotation().Yaw, 0.0f) };

		// UE_LOG(LogGIE, Log, TEXT("Recieved Movement Input(%s)"), *Value.ToString());

		if (Value.X != 0.0f)
		{
			const auto MovementDirection{ MovementRotation.RotateVector(FVector::RightVector) };
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const auto MovementDirection{ MovementRotation.RotateVector(FVector::ForwardVector) };
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UPlayableComponent::NativeInput_LookMouse(const FInputActionValue& InputActionValue)
{
	if (auto* Pawn{ GetPawn<APawn>() })
	{
		const auto Value{ InputActionValue.Get<FVector2D>() };

		// UE_LOG(LogGIE, Log, TEXT("Recieved Look Input(%s)"), *Value.ToString());

		if (Value.X != 0.0f)
		{
			Pawn->AddControllerYawInput(Value.X);
		}

		if (Value.Y != 0.0f)
		{
			Pawn->AddControllerPitchInput(Value.Y);
		}
	}
}

void UPlayableComponent::NativeInput_LookStick(const FInputActionValue& InputActionValue)
{
	if (auto* Pawn{ GetPawn<APawn>() })
	{
		const auto Value{ InputActionValue.Get<FVector2D>() };

		const auto* World{ GetWorld() };
		check(World);

		if (Value.X != 0.0f)
		{
			static constexpr auto LookYawRate{ 300.0f };

			Pawn->AddControllerYawInput(Value.X * LookYawRate * World->GetDeltaSeconds());
		}

		if (Value.Y != 0.0f)
		{
			static constexpr auto LookPitchRate{ 165.0f };

			Pawn->AddControllerPitchInput(Value.Y * LookPitchRate * World->GetDeltaSeconds());
		}
	}
}


UPlayableComponent* UPlayableComponent::FindPlayableComponent(const APawn* Pawn)
{
	return (Pawn ? Pawn->FindComponentByClass<UPlayableComponent>() : nullptr);
}
