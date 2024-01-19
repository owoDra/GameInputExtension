// Copyright (C) 2024 owoDra

#pragma once

#include "EnhancedInputComponent.h"

#include "GameplayTagContainer.h"

#include "InputProcessComponent.generated.h"

class UInputProcessor;


/**
 * EnhancedInputComponent with additional InputProcessor functionality
 */
UCLASS(Config = Input)
class UInputProcessComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	UInputProcessComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//
	// Name of the event that signals that Actor's InputCompnent is ready.
	//
	static const FName NAME_InputComponentReady;

public:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Processors")
	TMap<TSubclassOf<UInputProcessor>, TObjectPtr<UInputProcessor>> Processors;

public:
	UFUNCTION(BlueprintCallable, Category = "Processors")
	void AddInputProcessor(TSubclassOf<UInputProcessor> InClass);

	UFUNCTION(BlueprintCallable, Category = "Processors")
	void RemoveAllInputProcessors();

};
