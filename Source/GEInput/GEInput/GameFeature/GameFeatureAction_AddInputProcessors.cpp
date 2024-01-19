// Copyright (C) 2024 owoDra

#include "GameFeatureAction_AddInputProcessors.h"

#include "InputProcessComponent.h"

#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Actor.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddInputProcessors)

////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "GameFeatures"

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddInputProcessors::IsDataValid(FDataValidationContext& Context) const
{
	auto Result{ CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid) };

	auto Index{ 0 };
	for (const auto& Entry : InputProcessors)
	{
		if (Entry.ActorClass.IsNull())
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);

			Context.AddError(FText::Format(LOCTEXT("NullInputConfig", "Null ActorClass at index {0}."), Index));
		}

		++Index;
	}

	return Result;
}
#endif


void UGameFeatureAction_AddInputProcessors::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	auto& ActiveData{ ContextData.FindOrAdd(Context) };

	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) || !ensure(ActiveData.ActorsAddedTo.IsEmpty()))
	{
		Reset(ActiveData);
	}

	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputProcessors::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	auto* ActiveData{ ContextData.Find(Context) };

	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}


void UGameFeatureAction_AddInputProcessors::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	auto* World{ WorldContext.World() };
	auto GameInstance{ WorldContext.OwningGameInstance };
	auto& ActiveData{ ContextData.FindOrAdd(ChangeContext) };

	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		auto EntryIndex{ 0 };
		for (const auto& Entry : InputProcessors)
		{
			if (auto* Manager{ UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance) })
			{
				auto NewDelegate{ UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandleActorExtension, EntryIndex, ChangeContext) };
				auto ExtensionRequestHandle{ Manager->AddExtensionHandler(Entry.ActorClass, NewDelegate) };

				ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
			}

			EntryIndex++;
		}
	}
}


void UGameFeatureAction_AddInputProcessors::Reset(FPerContextData& ActiveData)
{
	ActiveData.ExtensionRequestHandles.Empty();

	while (!ActiveData.ActorsAddedTo.IsEmpty())
	{
		auto ActorPtr{ ActiveData.ActorsAddedTo.Top() };

		if (ActorPtr.IsValid())
		{
			RemoveInputProcessorsForActor(ActorPtr.Get(), ActiveData);
		}
		else
		{
			ActiveData.ActorsAddedTo.Pop();
		}
	}
}

void UGameFeatureAction_AddInputProcessors::HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex, FGameFeatureStateChangeContext ChangeContext)
{
	auto& ActiveData{ ContextData.FindOrAdd(ChangeContext) };

	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		AddInputProcessorsForActor(Actor, InputProcessors[EntryIndex], ActiveData);
	}
	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UInputProcessComponent::NAME_InputComponentReady))
	{
		RemoveInputProcessorsForActor(Actor, ActiveData);
	}
}

void UGameFeatureAction_AddInputProcessors::AddInputProcessorsForActor(AActor* Actor, const FInputProcessorsToAdd& InputProcessorsToAdd, FPerContextData& ActiveData)
{
	auto* InputComponent{ Cast<UInputProcessComponent>(Actor->InputComponent) };
	InputComponent = InputComponent ? InputComponent : Actor->FindComponentByClass<UInputProcessComponent>();

	if (InputComponent)
	{
		for (const auto& Entry : InputProcessorsToAdd.Processors)
		{
			auto* ProcessorToAdd{ Entry.IsValid() ? Entry.Get() : Entry.LoadSynchronous() };

			InputComponent->AddInputProcessor(ProcessorToAdd);
		}
	}
}

void UGameFeatureAction_AddInputProcessors::RemoveInputProcessorsForActor(AActor* Actor, FPerContextData& ActiveData)
{
	auto* InputComponent{ Cast<UInputProcessComponent>(Actor->InputComponent) };
	InputComponent = InputComponent ? InputComponent : Actor->FindComponentByClass<UInputProcessComponent>();

	if (InputComponent)
	{
		InputComponent->RemoveAllInputProcessors();
	}
}

#undef LOCTEXT_NAMESPACE
