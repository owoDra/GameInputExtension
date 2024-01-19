// Copyright (C) 2024 owoDra

#include "InputProcessor_MoveAndLook.h"

#include "InputProcessComponent.h"
#include "GameplayTag/GEInputTags_Input.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InputProcessor_MoveAndLook)


UInputProcessor_MoveAndLook::UInputProcessor_MoveAndLook(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bBind_Triggered = true;
	bBind_Started = false;
	bBind_Ongoing = false;
	bBind_Canceled = false;
	bBind_Complete = false;

	InputActions.Emplace(TAG_Input_Gamepad_Look, nullptr);
	InputActions.Emplace(TAG_Input_Gamepad_Move, nullptr);
	InputActions.Emplace(TAG_Input_MouseAndKeyboard_Look, nullptr);
	InputActions.Emplace(TAG_Input_MouseAndKeyboard_Move, nullptr);
}


void UInputProcessor_MoveAndLook::OnInitialized_Implementation(UInputProcessComponent* InputComponent)
{
	auto* Owner{ InputComponent->GetOwner() };

	if (auto* PC{ Cast<APlayerController>(Owner) })
	{
		Pawn = PC->GetPawn();
	}
	else
	{
		Pawn = Cast<APawn>(Owner);
	}
}

void UInputProcessor_MoveAndLook::OnDeinitialize_Implementation(UInputProcessComponent* InputComponent)
{
	Pawn.Reset();
}

void UInputProcessor_MoveAndLook::OnTriggered_Implementation(const FGameplayTag& InputTag, const FInputActionValue& InputActionValue)
{
	if (Pawn.IsValid())
	{
		if (InputTag == TAG_Input_Gamepad_Move || InputTag == TAG_Input_MouseAndKeyboard_Move)
		{
			Input_Move(InputActionValue);
		}
		else if (InputTag == TAG_Input_Gamepad_Look)
		{
			Input_LookPad(InputActionValue);
		}
		else if (InputTag == TAG_Input_MouseAndKeyboard_Look)
		{
			Input_LookMouse(InputActionValue);
		}
	}
}


void UInputProcessor_MoveAndLook::Input_Move(const FInputActionValue& InputActionValue)
{
	const auto Value{ InputActionValue.Get<FVector2D>() };
	const auto MovementRotation{ FRotator(0.0f, Pawn->GetViewRotation().Yaw, 0.0f)};

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

void UInputProcessor_MoveAndLook::Input_LookMouse(const FInputActionValue& InputActionValue)
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

void UInputProcessor_MoveAndLook::Input_LookPad(const FInputActionValue& InputActionValue)
{
	const auto Value{ InputActionValue.Get<FVector2D>() };

	const auto* World{ GetWorld() };
	check(World);

	const auto DeltaSeconds{ World->GetDeltaSeconds() };

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * PadLookYawRate * DeltaSeconds);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * PadLookPitchRate * DeltaSeconds);
	}
}
