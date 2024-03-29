﻿// Copyright (C) 2024 owoDra

#pragma once

#include "GameplayTagContainer.h"

#include "InputProcessor.generated.h"

class UInputProcessComponent;
class UInputAction;


/**
 * Class for performing specific input processing of actors
 */
UCLASS(Abstract, Blueprintable)
class GEINPUT_API UInputProcessor : public UObject
{
	GENERATED_BODY()
public:
	UInputProcessor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input Process", meta = (ForceInlineRow, Categories = "Input"))
	TMap<FGameplayTag, TObjectPtr<UInputAction>> InputActions;

	UPROPERTY(EditDefaultsOnly, Category = "Input Process|Bind")
	bool bBind_Triggered{ false };

	UPROPERTY(EditDefaultsOnly, Category = "Input Process|Bind")
	bool bBind_Started{ true };

	UPROPERTY(EditDefaultsOnly, Category = "Input Process|Bind")
	bool bBind_Ongoing{ false };

	UPROPERTY(EditDefaultsOnly, Category = "Input Process|Bind")
	bool bBind_Canceled{ false };

	UPROPERTY(EditDefaultsOnly, Category = "Input Process|Bind")
	bool bBind_Complete{ true };

public:
	void Initialize(UInputProcessComponent* InputComponent);
	void Deinitialize(UInputProcessComponent* InputComponent);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Initialization")
	void OnInitialized(UInputProcessComponent* InputComponent);
	virtual void OnInitialized_Implementation(UInputProcessComponent* InputComponent) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Initialization")
	void OnDeinitialize(UInputProcessComponent* InputComponent);
	virtual void OnDeinitialize_Implementation(UInputProcessComponent* InputComponent) {}


protected:
	void HandleTriggered(const FInputActionValue& InputActionValue, FGameplayTag InputTag);
	void HandleStarted(const FInputActionValue& InputActionValue, FGameplayTag InputTag);
	void HandleOngoing(const FInputActionValue& InputActionValue, FGameplayTag InputTag);
	void HandleCanceled(const FInputActionValue& InputActionValue, FGameplayTag InputTag);
	void HandleComplete(const FInputActionValue& InputActionValue, FGameplayTag InputTag);

	UFUNCTION(BlueprintNativeEvent, Category = "Process")
	void OnTriggered(const FGameplayTag& InputTag, const FInputActionValue& InputActionValue);
	virtual void OnTriggered_Implementation(const FGameplayTag& InputTag, const FInputActionValue& InputActionValue) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Process")
	void OnStarted(const FGameplayTag& InputTag, const FInputActionValue& InputActionValue);
	virtual void OnStarted_Implementation(const FGameplayTag& InputTag, const FInputActionValue& InputActionValue) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Process")
	void OnOngoing(const FGameplayTag& InputTag, const FInputActionValue& InputActionValue);
	virtual void OnOngoing_Implementation(const FGameplayTag& InputTag, const FInputActionValue& InputActionValue) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Process")
	void OnCanceled(const FGameplayTag& InputTag, const FInputActionValue& InputActionValue);
	virtual void OnCanceled_Implementation(const FGameplayTag& InputTag, const FInputActionValue& InputActionValue) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Process")
	void OnComplete(const FGameplayTag& InputTag, const FInputActionValue& InputActionValue);
	virtual void OnComplete_Implementation(const FGameplayTag& InputTag, const FInputActionValue& InputActionValue) {}

};
