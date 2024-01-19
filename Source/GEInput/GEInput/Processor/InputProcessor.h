// Copyright (C) 2024 owoDra

#pragma once

#include "GameplayTagContainer.h"

#include "InputProcessor.generated.h"

class UCoreInputComponent;
class UInputAction;


/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GEINPUT_API UInputProcessor : public UObject
{
	GENERATED_BODY()
public:
	UInputProcessor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input Process")
	TMap<FGameplayTag, TObjectPtr<UInputAction>> InputActions;

	UPROPERTY(EditDefaultsOnly, Category = "Input Process")
	bool bBind_Triggered{ false };

	UPROPERTY(EditDefaultsOnly, Category = "Input Process")
	bool bBind_Started{ true };

	UPROPERTY(EditDefaultsOnly, Category = "Input Process")
	bool bBind_Ongoing{ false };

	UPROPERTY(EditDefaultsOnly, Category = "Input Process")
	bool bBind_Canceled{ false };

	UPROPERTY(EditDefaultsOnly, Category = "Input Process")
	bool bBind_Complete{ true };

public:
	void Initialize(UCoreInputComponent* InputComponent);
	void Deinitialize(UCoreInputComponent* InputComponent);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Initialization")
	void OnInitialized(UCoreInputComponent* InputComponent);

	UFUNCTION(BlueprintImplementableEvent, Category = "Initialization")
	void OnDeinitialize(UCoreInputComponent* InputComponent);


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
