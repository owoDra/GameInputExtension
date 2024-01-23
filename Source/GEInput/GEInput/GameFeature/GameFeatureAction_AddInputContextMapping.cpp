// Copyright (C) 2024 owoDra

#include "GameFeatureAction_AddInputContextMapping.h"

#include "InputProcessComponent.h"

#include "AssetManager/GFCAssetManager.h"

#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Components/GameFrameworkComponentManager.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddInputContextMapping)

///////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "GameFeatures"

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddInputContextMapping::IsDataValid(FDataValidationContext& Context) const
{
	auto Result{ CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid) };

	int32 Index{ 0 };
	for (const auto& Entry : InputMappings)
	{
		if (Entry.InputMapping.IsNull())
		{
			Result = EDataValidationResult::Invalid;

			Context.AddError(FText::Format(LOCTEXT("NullInputMapping", "Null InputMapping at index {0}."), Index));
		}
		++Index;
	}

	return Result;
}
#endif


void UGameFeatureAction_AddInputContextMapping::OnGameFeatureRegistering()
{
	Super::OnGameFeatureRegistering();

	RegisterInputMappingContexts();
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	auto& ActiveData{ ContextData.FindOrAdd(Context) };

	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) ||
		!ensure(ActiveData.ControllersAddedTo.IsEmpty()))
	{
		Reset(ActiveData);
	}

	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	auto* ActiveData{ ContextData.Find(Context) };

	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureUnregistering()
{
	Super::OnGameFeatureUnregistering();

	UnregisterInputMappingContexts();
}


void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContexts()
{
	RegisterInputContextMappingsForGameInstanceHandle = FWorldDelegates::OnStartGameInstance.AddUObject(this, &UGameFeatureAction_AddInputContextMapping::RegisterInputContextMappingsForGameInstance);

	const auto& WorldContexts{ GEngine->GetWorldContexts() };

	for (auto WorldContextIterator{ WorldContexts.CreateConstIterator() }; WorldContextIterator; ++WorldContextIterator)
	{
		RegisterInputContextMappingsForGameInstance(WorldContextIterator->OwningGameInstance);
	}
}

void UGameFeatureAction_AddInputContextMapping::RegisterInputContextMappingsForGameInstance(UGameInstance* GameInstance)
{
	if (GameInstance != nullptr && !GameInstance->OnLocalPlayerAddedEvent.IsBoundToObject(this))
	{
		GameInstance->OnLocalPlayerAddedEvent.AddUObject(this, &UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer);
		GameInstance->OnLocalPlayerRemovedEvent.AddUObject(this, &UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer);

		for (auto LocalPlayerIterator{ GameInstance->GetLocalPlayerIterator() }; LocalPlayerIterator; ++LocalPlayerIterator)
		{
			RegisterInputMappingContextsForLocalPlayer(*LocalPlayerIterator);
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
	ensure(LocalPlayer);

	auto* EISubsystem{ ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer) };
	auto* Settings{ EISubsystem ? EISubsystem->GetUserSettings() : nullptr};

	if (Settings)
	{
		auto& AssetManager{ UGFCAssetManager::Get() };

		for (const auto& Entry : InputMappings)
		{
			// Skip entries that don't want to be registered

			if (!Entry.bRegisterWithSettings)
			{
				continue;
			}

			// Register this IMC with the settings!

			if (auto* IMC{ AssetManager.GetAsset(Entry.InputMapping) })
			{
				Settings->RegisterInputMappingContext(IMC);
			}
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContexts()
{
	FWorldDelegates::OnStartGameInstance.Remove(RegisterInputContextMappingsForGameInstanceHandle);
	RegisterInputContextMappingsForGameInstanceHandle.Reset();

	const auto& WorldContexts{ GEngine->GetWorldContexts() };

	for (auto WorldContextIterator{ WorldContexts.CreateConstIterator() }; WorldContextIterator; ++WorldContextIterator)
	{
		UnregisterInputContextMappingsForGameInstance(WorldContextIterator->OwningGameInstance);
	}
}

void UGameFeatureAction_AddInputContextMapping::UnregisterInputContextMappingsForGameInstance(UGameInstance* GameInstance)
{
	if (GameInstance != nullptr)
	{
		GameInstance->OnLocalPlayerAddedEvent.RemoveAll(this);
		GameInstance->OnLocalPlayerRemovedEvent.RemoveAll(this);

		for (auto LocalPlayerIterator{ GameInstance->GetLocalPlayerIterator() }; LocalPlayerIterator; ++LocalPlayerIterator)
		{
			UnregisterInputMappingContextsForLocalPlayer(*LocalPlayerIterator);
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
	ensure(LocalPlayer);

	auto* EISubsystem{ ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer) };
	auto* Settings{ EISubsystem ? EISubsystem->GetUserSettings() : nullptr };

	if (Settings)
	{
		for (const auto& Entry : InputMappings)
		{
			// Skip entries that don't want to be registered

			if (!Entry.bRegisterWithSettings)
			{
				continue;
			}

			// Register this IMC with the settings!

			if (auto* IMC{ Entry.InputMapping.Get() })
			{
				Settings->UnregisterInputMappingContext(IMC);
			}
		}
	}
}


void UGameFeatureAction_AddInputContextMapping::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	auto* World{ WorldContext.World() };
	const auto bIsGameWorld{ World ? World->IsGameWorld() : false };

	auto GameInstance{ WorldContext.OwningGameInstance };
	auto* ComponentManager{ UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance) };

	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if (ComponentManager && bIsGameWorld)
	{
		auto AddAbilitiesDelegate{ UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandleControllerExtension, ChangeContext) };
		auto ExtensionRequestHandle{ ComponentManager->AddExtensionHandler(APlayerController::StaticClass(), AddAbilitiesDelegate) };

		ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
	}
}

void UGameFeatureAction_AddInputContextMapping::Reset(FPerContextData& ActiveData)
{
	ActiveData.ExtensionRequestHandles.Empty();

	while (!ActiveData.ControllersAddedTo.IsEmpty())
	{
		auto ControllerPtr{ ActiveData.ControllersAddedTo.Top() };

		if (ControllerPtr.IsValid())
		{
			RemoveInputMapping(ControllerPtr.Get(), ActiveData);
		}
		else
		{
			ActiveData.ControllersAddedTo.Pop();
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::HandleControllerExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	auto* AsController{ CastChecked<APlayerController>(Actor) };
	auto& ActiveData{ ContextData.FindOrAdd(ChangeContext) };

	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveInputMapping(AsController, ActiveData);
	}
	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UInputProcessComponent::NAME_InputComponentReady))
	{
		AddInputMappingForPlayer(AsController, ActiveData);
	}
}

void UGameFeatureAction_AddInputContextMapping::AddInputMappingForPlayer(APlayerController* PlayerController, FPerContextData& ActiveData)
{
	if (auto* LocalPlayer{ PlayerController->GetLocalPlayer() })
	{
		if (auto* InputSystem{ LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() })
		{
			for (const auto& Entry : InputMappings)
			{
				if (const auto* IMC{ Entry.InputMapping.Get() })
				{
					InputSystem->AddMappingContext(IMC, Entry.Priority);
				}
			}
		}
		else
		{
			UE_LOG(LogGameFeatures, Error, TEXT("Failed to find `UEnhancedInputLocalPlayerSubsystem` for local player. Input mappings will not be added. Make sure you're set to use the EnhancedInput system via config file."));
		}
	}

	ActiveData.ControllersAddedTo.Add(PlayerController);
}

void UGameFeatureAction_AddInputContextMapping::RemoveInputMapping(APlayerController* PlayerController, FPerContextData& ActiveData)
{
	if (auto* LocalPlayer{ PlayerController->GetLocalPlayer() })
	{
		if (auto* InputSystem{ LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() })
		{
			for (const auto& Entry : InputMappings)
			{
				if (const auto* IMC{ Entry.InputMapping.Get() })
				{
					InputSystem->RemoveMappingContext(IMC);
				}
			}
		}
	}

	ActiveData.ControllersAddedTo.Remove(PlayerController);
}

#undef LOCTEXT_NAMESPACE
