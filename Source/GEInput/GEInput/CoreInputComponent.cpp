// Copyright (C) 2024 owoDra

#include "CoreInputComponent.h"

#include "Processor/InputProcessor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CoreInputComponent)


UCoreInputComponent::UCoreInputComponent(const FObjectInitializer& ObjectInitializer)
{
}


void UCoreInputComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllInputProcessors();

	Super::EndPlay(EndPlayReason);
}


void UCoreInputComponent::AddInputProcessors(TSubclassOf<UInputProcessor> InClass)
{
	auto* Owner{ GetOwner() };
	check(Owner);

	// Suspend if class is not valid

	if (!InClass)
	{
		return;
	}

	// Suspend if class already exist

	for (const auto& Processor : Processors)
	{
		if (Processor->GetClass() == InClass)
		{
			return;
		}
	}
	
	// Create new processor

	auto* NewProcessor{ NewObject<UInputProcessor>(Owner, InClass) };
	NewProcessor->Initialize(this);

	Processors.Emplace(NewProcessor);
}

void UCoreInputComponent::RemoveAllInputProcessors()
{
	for (const auto& Processor : Processors)
	{
		Processor->Deinitialize(this);
	}

	Processors.Empty();
}
