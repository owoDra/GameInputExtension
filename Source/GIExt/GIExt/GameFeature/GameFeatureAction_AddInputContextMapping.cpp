// Copyright (C) 2024 owoDra

#include "GameFeatureAction_AddInputContextMapping.h"

#include "PlayableComponent.h"

#include "Components/GameFrameworkComponentManager.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddInputContextMapping)


#define LOCTEXT_NAMESPACE "GameFeatures"

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddInputContextMapping::IsDataValid(TArray<FText>& ValidationErrors)
{
	auto Result{ CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid) };

	auto Index{ 0 };
	for (const auto& Mapping : InputMappings)
	{
		if (Mapping.InputMapping.IsNull())
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
			ValidationErrors.Add(FText::Format(LOCTEXT("NullInputMapping", "Null InputMapping at index {0}."), Index));
		}

		++Index;
	}

	return Result;
}
#endif


void UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	auto& ActiveData{ ContextData.FindOrAdd(Context) };

	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) || !ensure(ActiveData.ControllersAddedTo.IsEmpty()))
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


void UGameFeatureAction_AddInputContextMapping::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	auto* World{ WorldContext.World() };
	auto GameInstance{ WorldContext.OwningGameInstance };
	auto& ActiveData{ ContextData.FindOrAdd(ChangeContext) };

	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		if (auto* Manager{ UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance) })
		{
			auto AddInputContextMappingDelegate{ UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandleControllerExtension, ChangeContext) };
			auto ExtensionRequestHandle{ Manager->AddExtensionHandler(APlayerController::StaticClass(), AddInputContextMappingDelegate) };

			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
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
	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UPlayableComponent::NAME_BindInputsNow))
	{
		AddInputMappingForPlayer(AsController->GetLocalPlayer(), ActiveData);
	}
}

void UGameFeatureAction_AddInputContextMapping::AddInputMappingForPlayer(UPlayer* Player, FPerContextData& ActiveData)
{
	if (auto* LocalPlayer{ Cast<ULocalPlayer>(Player) })
	{
		if (auto* InputSystem{ LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() })
		{
			for (const auto& Mapping : InputMappings)
			{
				if (const auto* IMC{ Mapping.InputMapping.Get() })
				{
					InputSystem->AddMappingContext(IMC, Mapping.Priority);
				}
			}
		}
		else
		{
			UE_LOG(LogGameFeatures, Error, TEXT("Failed to find `UEnhancedInputLocalPlayerSubsystem` for local player. Input mappings will not be added. Make sure you're set to use the EnhancedInput system via config file."));
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::RemoveInputMapping(APlayerController* PlayerController, FPerContextData& ActiveData)
{
	if (auto* LocalPlayer{ PlayerController->GetLocalPlayer() })
	{
		if (auto* InputSystem{ LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() })
		{
			for (const auto& Mapping : InputMappings)
			{
				if (const auto* IMC{ Mapping.InputMapping.Get() })
				{
					InputSystem->RemoveMappingContext(IMC);
				}
			}
		}
	}

	ActiveData.ControllersAddedTo.Remove(PlayerController);
}

#undef LOCTEXT_NAMESPACE