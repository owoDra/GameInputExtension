// Copyright (C) 2024 owoDra

#pragma once

#include "EnhancedInputComponent.h"

#include "GameplayTagContainer.h"

#include "CoreInputComponent.generated.h"

class UInputProcessor;

/**
 * EnhancedInputComponent with additional InputProcessor functionality
 */
UCLASS(Config = Input)
class UCoreInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UCoreInputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Processors")
	TArray<TObjectPtr<UInputProcessor>> Processors;

public:
	UFUNCTION(BlueprintCallable, Category = "Processors")
	void AddInputProcessors(TSubclassOf<UInputProcessor> InClass);

	UFUNCTION(BlueprintCallable, Category = "Processors")
	void RemoveAllInputProcessors();

};
