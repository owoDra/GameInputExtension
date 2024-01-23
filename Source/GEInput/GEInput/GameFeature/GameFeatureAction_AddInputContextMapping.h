// Copyright (C) 2024 owoDra

#pragma once

#include "GameFeature/GameFeatureAction_WorldActionBase.h"

#include "GameFeatureAction_AddInputContextMapping.generated.h"

class UInputMappingContext;
class APlayerController;
struct FComponentRequestHandle;


/**
 * Adds InputMappingContext to local players' EnhancedInput system.
 * Expects that local players are set up to use the EnhancedInput system.
 */
USTRUCT()
struct FInputMappingContextAndPriority
{
	GENERATED_BODY()
public:
	FInputMappingContextAndPriority() {}

public:
	UPROPERTY(EditAnywhere, Category = "Input", meta = (AssetBundles = "Client, Server"))
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	//
	// Higher priority input mappings will be prioritized over mappings with a lower priority.
	//
	UPROPERTY(EditAnywhere, Category = "Input")
	int32 Priority{ 0 };

	//
	// If true, then this mapping context will be registered with the settings when this game feature action is registered. 
	//
	UPROPERTY(EditAnywhere, Category = "Input")
	bool bRegisterWithSettings{ true };
};


/**
 * Adds InputMappingContext to local players' EnhancedInput system.
 * Expects that local players are set up to use the EnhancedInput system.
 */
UCLASS(meta = (DisplayName = "Add Input Mapping"))
class UGameFeatureAction_AddInputContextMapping final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
public:
	UGameFeatureAction_AddInputContextMapping() {}

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

protected:
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<APlayerController>> ControllersAddedTo;
	};

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	//
	// Delegate for when the game instance is changed to register IMC's
	//
	FDelegateHandle RegisterInputContextMappingsForGameInstanceHandle;

protected:
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<FInputMappingContextAndPriority> InputMappings;

public:
	virtual void OnGameFeatureRegistering() override;
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void OnGameFeatureUnregistering() override;

private:
	/** 
	 * Registers owned Input Mapping Contexts to the Input Registry Subsystem. 
	 * Also binds onto the start of GameInstances and the adding/removal of Local Players. 
	 */
	void RegisterInputMappingContexts();

	/** 
	 * Registers owned Input Mapping Contexts to the Input Registry Subsystem for a specified GameInstance. 
	 * This also gets called by a GameInstance Start. 
	 */
	void RegisterInputContextMappingsForGameInstance(UGameInstance* GameInstance);

	/** 
	 * Registers owned Input Mapping Contexts to the Input Registry Subsystem for a specified Local Player. 
	 * This also gets called when a Local Player is added. 
	 */
	void RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);

	/** 
	 * Unregisters owned Input Mapping Contexts from the Input Registry Subsystem. 
	 * Also unbinds from the start of GameInstances and the adding/removal of Local Players. 
	 */
	void UnregisterInputMappingContexts();

	/** 
	 * Unregisters owned Input Mapping Contexts from the Input Registry Subsystem for a specified GameInstance. 
	 */
	void UnregisterInputContextMappingsForGameInstance(UGameInstance* GameInstance);

	/** 
	 * Unregisters owned Input Mapping Contexts from the Input Registry Subsystem for a specified Local Player. 
	 * This also gets called when a Local Player is removed. 
	 */
	void UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);

	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

	void Reset(FPerContextData& ActiveData);
	void HandleControllerExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
	void AddInputMappingForPlayer(APlayerController* PlayerController, FPerContextData& ActiveData);
	void RemoveInputMapping(APlayerController* PlayerController, FPerContextData& ActiveData);

};
