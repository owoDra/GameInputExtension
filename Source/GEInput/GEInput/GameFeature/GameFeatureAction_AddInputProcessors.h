// Copyright (C) 2024 owoDra

#pragma once

#include "GameFeature/GameFeatureAction_WorldActionBase.h"

#include "GameFeatureAction_AddInputProcessors.generated.h"

class UInputProcessor;
class AActor;
struct FComponentRequestHandle;


/**
 * Entry data of InputProcessor to be added
 */
USTRUCT()
struct FInputProcessorsToAdd
{
	GENERATED_BODY()
public:
	FInputProcessorsToAdd() {}

public:
	//
	// The base actor class to add to
	//
	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftClassPtr<AActor> ActorClass;

	//
	// List of Processors to add to actors of the specified class
	//
	UPROPERTY(EditAnywhere, Category = "Input", meta = (AssetBundles = "Client,Server"))
	TArray<TSoftClassPtr<UInputProcessor>> Processors;
};


/**
 * 
 */
UCLASS(meta = (DisplayName = "Add Input Processor"))
class UGameFeatureAction_AddInputProcessors final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
public:
	UGameFeatureAction_AddInputProcessors() {}

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const;
#endif // WITH_EDITOR

private:
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<AActor>> ActorsAddedTo;
	};

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

protected:
	UPROPERTY(EditAnywhere, Category = "Input", meta = (AssetBundles = "Client,Server"))
	TArray<FInputProcessorsToAdd> InputProcessors;

public:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

private:
	void Reset(FPerContextData& ActiveData);
	void HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex, FGameFeatureStateChangeContext ChangeContext);
	void AddInputProcessorsForActor(AActor* Actor, const FInputProcessorsToAdd& InputProcessorsToAdd, FPerContextData& ActiveData);
	void RemoveInputProcessorsForActor(AActor* Actor, FPerContextData& ActiveData);

};
