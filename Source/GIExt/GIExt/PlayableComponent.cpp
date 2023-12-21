// Copyright (C) 2023 owoDra

#include "PlayableComponent.h"

#include "GameplayTag/GIETags_Input.h"
#include "InputConfig.h"
#include "GIExtLogs.h"

#include "InitState/InitStateTags.h"

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


void UPlayableComponent::OnRegister()
{
	Super::OnRegister();

	// This component can only be added to classes derived from APawn

	const auto* Pawn{ GetPawn<APawn>() };
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("PlayableComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	// No more than two of these components should be added to a Actor.

	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(UPlayableComponent::StaticClass(), Components);
	ensureAlwaysMsgf((Components.Num() == 1), TEXT("Only one PlayableComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	// Register this component in the GameFrameworkComponentManager.

	RegisterInitStateFeature();
}

void UPlayableComponent::BeginPlay()
{
	Super::BeginPlay();

	// Start listening for changes in the initialization state of all features 
	// related to the Pawn that owns this component.

	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// Change the initialization state of this component to [Spawned]

	ensure(TryToChangeInitState(TAG_InitState_Spawned));

	// Check if initialization process can continue

	CheckDefaultInitialization();
}

void UPlayableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}


void UPlayableComponent::InitializePlayerInput()
{
	if (!DefaultInputConfig)
	{
		return;
	}

	// Check essential values

	const auto* Pawn{ GetPawnChecked<APawn>() };

	auto* IC{ Cast<UEnhancedInputComponent>(Pawn->InputComponent) };
	check(IC);

	const auto* PC{ GetController<APlayerController>() };
	check(PC);

	const auto* LP{ PC->GetLocalPlayer() };
	check(LP);

	auto* Subsystem{ LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() };
	check(Subsystem);

	// Initialize Input mappings

	Subsystem->ClearAllMappings();

	TArray<uint32> BindHandles;
	DefaultInputConfig->BindTagActions(IC, this, &ThisClass::TagInput_Pressed, &ThisClass::TagInput_Released, /*out*/ BindHandles);

	DefaultInputConfig->BindNativeAction(IC, TAG_Input_Move_KM, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_Move);
	DefaultInputConfig->BindNativeAction(IC, TAG_Input_Move_Pad, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_Move);
	DefaultInputConfig->BindNativeAction(IC, TAG_Input_Look_KM, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_LookMouse);
	DefaultInputConfig->BindNativeAction(IC, TAG_Input_Look_Pad, ETriggerEvent::Triggered, this, &ThisClass::NativeInput_LookStick);

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}


bool UPlayableComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	auto* Pawn{ GetPawn<APawn>() };

	/**
	 * [InitState None] -> [InitState Spawned]
	 */
	if (!CurrentState.IsValid() && DesiredState == TAG_InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}

	/**
	 * [InitState Spawned] -> [InitState DataAvailable]
	 */
	else if (CurrentState == TAG_InitState_Spawned && DesiredState == TAG_InitState_DataAvailable)
	{
		// Check if it is not SimulationProxy

		if (Pawn->GetLocalRole() == ROLE_SimulatedProxy)
		{
			return false;
		}

		// Check if it is not a bot player

		const auto bIsLocallyControlled{ Pawn->IsLocallyControlled() };
		const auto bIsBot{ Pawn->IsBotControlled() };

		if (!bIsLocallyControlled || bIsBot)
		{
			return false;
		}

		// Check has valid input component

		if (!Pawn->InputComponent)
		{
			return false;
		}

		return true;
	}

	/**
	 * [InitState DataAvailable] -> [InitState DataInitialized]
	 */
	else if (CurrentState == TAG_InitState_DataAvailable && DesiredState == TAG_InitState_DataInitialized)
	{
		if (DefaultInputConfig)
		{
			return true;
		}
	}

	/**
	 * [InitState DataInitialized] -> [InitState GameplayReady]
	 */
	else if (CurrentState == TAG_InitState_DataInitialized && DesiredState == TAG_InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UPlayableComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	UE_LOG(LogGIE, Log, TEXT("[%s] Playable Component InitState Reached: %s"),
		GetOwner()->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"), *DesiredState.GetTagName().ToString());

	/**
	 * [InitState DataAvailable] -> [InitState DataInitialized]
	 */
	if (CurrentState == TAG_InitState_DataAvailable && DesiredState == TAG_InitState_DataInitialized)
	{
		// Do only local

		if (GetPawnChecked<APawn>()->IsLocallyControlled())
		{
			InitializePlayerInput();
		}
	}
}

void UPlayableComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
}

void UPlayableComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain
	{
		TAG_InitState_Spawned,
		TAG_InitState_DataAvailable,
		TAG_InitState_DataInitialized,
		TAG_InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
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
	TagInput_PressedExtra(InputTag);
}

void UPlayableComponent::TagInput_Released(FGameplayTag InputTag)
{
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
