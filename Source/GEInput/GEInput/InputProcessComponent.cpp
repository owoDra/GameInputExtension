// Copyright (C) 2024 owoDra

#include "InputProcessComponent.h"

#include "Processor/InputProcessor.h"

#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InputProcessComponent)


const FName UInputProcessComponent::NAME_InputComponentReady("InputComponentReady");

UInputProcessComponent::UInputProcessComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UInputProcessComponent::OnRegister()
{
	Super::OnRegister();

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(GetOwner(), UInputProcessComponent::NAME_InputComponentReady);
}

void UInputProcessComponent::OnUnregister()
{
	RemoveAllInputProcessors();

	Super::OnUnregister();
}

void UInputProcessComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllInputProcessors();

	Super::EndPlay(EndPlayReason);
}


void UInputProcessComponent::AddInputProcessor(TSubclassOf<UInputProcessor> InClass)
{
	auto* Owner{ GetOwner() };
	check(Owner);

	// Suspend if class is not valid

	if (!InClass)
	{
		return;
	}

	// Suspend if class already exist

	if (Processors.Contains(InClass))
	{
		return;
	}
	
	// Create new processor

	auto* NewProcessor{ NewObject<UInputProcessor>(Owner, InClass) };
	NewProcessor->Initialize(this);

	Processors.Emplace(InClass, NewProcessor);
}

void UInputProcessComponent::RemoveAllInputProcessors()
{
	for (const auto& KVP : Processors)
	{
		if (auto Processor{ KVP.Value })
		{
			Processor->Deinitialize(this);
		}
	}

	Processors.Empty();
}
