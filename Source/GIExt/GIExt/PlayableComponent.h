// Copyright (C) 2024 owoDra

#pragma once

#include "Component/GFCPawnComponent.h"

#include "InputActionValue.h"

#include "PlayableComponent.generated.h"

class UInputConfig;
class AController;


/**
 * Component that initializes the player-controlled Pawn Input
 */
UCLASS(Meta=(BlueprintSpawnableComponent))
class GIEXT_API UPlayableComponent : public UGFCPawnComponent
{
	GENERATED_BODY()
public:
	UPlayableComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//
	// Function name used to add this component
	//
	static const FName NAME_ActorFeatureName;

	static const FName NAME_BindInputsNow;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitializePlayerInput(AController* Controller);
	virtual void UninitializePlayerInput(AController* Controller);

	/**
	 * Start listening for changes in the Pawn controller that owns this component
	 */
	void ListenControllerChange();

	/**
	 * Stop listening for changes in the controller of the Pawn that owns this component
	 */
	void UnlistenControllerChange();

	/**
	 * Run when the controller of the Pawn that owns this component is changed
	 */
	UFUNCTION()
	void HandleControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);


public:
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }

protected:
	virtual bool CanChangeInitStateToDataInitialized(UGameFrameworkComponentManager* Manager) const override;
	virtual void HandleChangeInitStateToDataInitialized(UGameFrameworkComponentManager* Manager) override;


protected:
	//
	// Current Input Config
	//
	UPROPERTY(EditAnywhere)
	TObjectPtr<const UInputConfig> DefaultInputConfig{ nullptr };

public:
	/**
	 * Set the current Input Config
	 */
	UFUNCTION(BlueprintCallable)
	void SetInputConfig(const UInputConfig* NewInputConfig);

public:
	void AddAdditionalInputConfig(const UInputConfig* InputConfig);
	void RemoveAdditionalInputConfig(const UInputConfig* InputConfig);


protected:
	void TagInput_Pressed(FGameplayTag InputTag);
	void TagInput_Released(FGameplayTag InputTag);

	virtual void TagInput_PressedExtra(FGameplayTag InputTag);
	virtual void TagInput_ReleasedExtra(FGameplayTag InputTag);

	void NativeInput_Move(const FInputActionValue& InputActionValue);
	void NativeInput_LookMouse(const FInputActionValue& InputActionValue);
	void NativeInput_LookStick(const FInputActionValue& InputActionValue);


public:
	UFUNCTION(BlueprintPure, Category = "Component")
	static UPlayableComponent* FindPlayableComponent(const APawn* Pawn);

};
