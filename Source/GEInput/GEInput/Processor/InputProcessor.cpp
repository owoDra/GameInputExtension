// Copyright (C) 2024 owoDra

#include "InputProcessor.h"

#include "CoreInputComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InputProcessor)


UInputProcessor::UInputProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UInputProcessor::Initialize(UCoreInputComponent* InputComponent)
{
	check(InputComponent);

	for (const auto& KVP : InputActions)
	{
		const auto& InputTag{ KVP.Key };
		const auto& InputAction{ KVP.Value };

		if (InputAction && InputTag.IsValid())
		{
			if (bBind_Triggered)
			{
				InputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &ThisClass::HandleTriggered, InputTag);
			}

			if (bBind_Started)
			{
				InputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &ThisClass::HandleStarted, InputTag);
			}

			if (bBind_Ongoing)
			{
				InputComponent->BindAction(InputAction, ETriggerEvent::Ongoing, this, &ThisClass::HandleOngoing, InputTag);
			}

			if (bBind_Canceled)
			{
				InputComponent->BindAction(InputAction, ETriggerEvent::Canceled, this, &ThisClass::HandleCanceled, InputTag);
			}

			if (bBind_Complete)
			{
				InputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &ThisClass::HandleComplete, InputTag);
			}
		}
	}

	OnInitialized(InputComponent);
}

void UInputProcessor::Deinitialize(UCoreInputComponent* InputComponent)
{
	OnDeinitialize(InputComponent);

	if (InputComponent)
	{
		InputComponent->ClearBindingsForObject(this);
	}
}


void UInputProcessor::HandleTriggered(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	OnTriggered(InputTag, InputActionValue);
}

void UInputProcessor::HandleStarted(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	OnStarted(InputTag, InputActionValue);
}

void UInputProcessor::HandleOngoing(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	OnOngoing(InputTag, InputActionValue);
}

void UInputProcessor::HandleCanceled(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	OnCanceled(InputTag, InputActionValue);
}

void UInputProcessor::HandleComplete(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	OnComplete(InputTag, InputActionValue);
}
