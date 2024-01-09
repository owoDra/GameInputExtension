// Copyright (C) 2024 owoDra

#include "GameFeatureAction_AddInputConfigs.h"

#include "PlayableComponent.h"

#include "Components/GameFrameworkComponentManager.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddInputConfigs)


#define LOCTEXT_NAMESPACE "GameFeatures"

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddInputConfigs::IsDataValid(TArray<FText>& ValidationErrors)
{
	auto Result{ CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid) };

	auto Index{ 0 };
	for (const auto& Entry : InputConfigs)
	{
		if (Entry.IsNull())
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);

			ValidationErrors.Add(FText::Format(LOCTEXT("NullInputConfig", "Null InputConfig at index {0}."), Index));
		}

		++Index;
	}

	return Result;
}
#endif


void UGameFeatureAction_AddInputConfigs::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	auto& ActiveData{ ContextData.FindOrAdd(Context) };

	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) || !ensure(ActiveData.PawnsAddedTo.IsEmpty()))
	{
		Reset(ActiveData);
	}

	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputConfigs::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	auto* ActiveData{ ContextData.Find(Context) };

	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}


void UGameFeatureAction_AddInputConfigs::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	auto* World{ WorldContext.World() };
	auto GameInstance{ WorldContext.OwningGameInstance };
	auto& ActiveData{ ContextData.FindOrAdd(ChangeContext) };

	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		if (auto* Manager{ UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance) })
		{
			auto AddAbilitiesDelegate{ UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandlePawnExtension, ChangeContext) };
			auto ExtensionRequestHandle{ Manager->AddExtensionHandler(APawn::StaticClass(), AddAbilitiesDelegate) };

			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}


void UGameFeatureAction_AddInputConfigs::Reset(FPerContextData& ActiveData)
{
	ActiveData.ExtensionRequestHandles.Empty();

	while (!ActiveData.PawnsAddedTo.IsEmpty())
	{
		auto PawnPtr{ ActiveData.PawnsAddedTo.Top() };

		if (PawnPtr.IsValid())
		{
			RemoveInputMapping(PawnPtr.Get(), ActiveData);
		}
		else
		{
			ActiveData.PawnsAddedTo.Pop();
		}
	}
}

void UGameFeatureAction_AddInputConfigs::HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	auto* AsPawn{ CastChecked<APawn>(Actor) };
	auto& ActiveData{ ContextData.FindOrAdd(ChangeContext) };

	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveInputMapping(AsPawn, ActiveData);
	}
	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UPlayableComponent::NAME_BindInputsNow))
	{
		AddInputMappingForPlayer(AsPawn, ActiveData);
	}
}

void UGameFeatureAction_AddInputConfigs::AddInputMappingForPlayer(APawn* Pawn, FPerContextData& ActiveData)
{
	auto* PlayerController{ Cast<APlayerController>(Pawn->GetController()) };

	if (auto* LocalPlayer{ PlayerController ? PlayerController->GetLocalPlayer() : nullptr })
	{
		if (auto* InputSystem{ LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() })
		{
			if (auto* PlayableComponent{ UPlayableComponent::FindPlayableComponent(Pawn) })
			{
				for (const auto& InputConfigSoftObj : InputConfigs)
				{
					const auto* InputConfig{ InputConfigSoftObj.IsValid() ? InputConfigSoftObj.Get() : InputConfigSoftObj.LoadSynchronous() };

					if (InputConfig)
					{
						PlayableComponent->AddAdditionalInputConfig(InputConfig);
					}
				}
			}

			ActiveData.PawnsAddedTo.AddUnique(Pawn);
		}
		else
		{
			UE_LOG(LogGameFeatures, Error, TEXT("Failed to find `UEnhancedInputLocalPlayerSubsystem` for local player. Input mappings will not be added. Make sure you're set to use the EnhancedInput system via config file."));
		}
	}
}

void UGameFeatureAction_AddInputConfigs::RemoveInputMapping(APawn* Pawn, FPerContextData& ActiveData)
{
	auto* PlayerController{ Cast<APlayerController>(Pawn->GetController()) };

	if (auto* LocalPlayer{ PlayerController ? PlayerController->GetLocalPlayer() : nullptr })
	{
		if (auto* InputSystem{ LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() })
		{
			if (auto* PlayableComponent{ UPlayableComponent::FindPlayableComponent(Pawn) })
			{
				for (const auto& InputConfigSoftObj : InputConfigs)
				{
					const auto* InputConfig{ InputConfigSoftObj.IsValid() ? InputConfigSoftObj.Get() : InputConfigSoftObj.LoadSynchronous() };

					if (InputConfig)
					{
						PlayableComponent->RemoveAdditionalInputConfig(InputConfig);
					}
				}
			}
		}
	}

	ActiveData.PawnsAddedTo.Remove(Pawn);
}

#undef LOCTEXT_NAMESPACE
