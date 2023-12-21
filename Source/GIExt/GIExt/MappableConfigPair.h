// Copyright (C) 2023 owoDra

#pragma once

#include "CommonInputBaseTypes.h"
#include "GameplayTagContainer.h"

#include "MappableConfigPair.generated.h"

class UPlayerMappableInputConfig;


/** 
 * A container to organize loaded player mappable configs to their CommonUI input type
 */
USTRUCT(BlueprintType)
struct FLoadedMappableConfigPair
{
	GENERATED_BODY()
public:
	FLoadedMappableConfigPair() {}

	FLoadedMappableConfigPair(const UPlayerMappableInputConfig* InConfig, ECommonInputType InType, const bool InIsActive)
		: Config(InConfig)
		, Type(InType)
		, bIsActive(InIsActive)
	{}

public:
	//
	// The player mappable input config that should be applied to the Enhanced Input subsystem
	//
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<const UPlayerMappableInputConfig> Config{ nullptr };

	//
	// The type of device that this mapping config should be applied to
	//
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECommonInputType Type{ ECommonInputType::Count };

	//
	// If this config is currently active. A config is marked as active when it's owning GFA is active
	//
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsActive{ false };

};


/** 
 * A container to organize potentially unloaded player mappable configs to their CommonUI input type 
 */
USTRUCT(BlueprintType)
struct FMappableConfigPair
{
	GENERATED_BODY()
public:
	FMappableConfigPair() {}

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UPlayerMappableInputConfig> Config;

	//
	// The type of config that this is. Useful for filtering out configs by the current input device
	// for things like the settings screen, or if you only want to apply this config when a certain
	// input type is being used.
	//
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECommonInputType Type{ ECommonInputType::Count };

	//
	// Container of platform traits that must be set in order for this input to be activated.
	// 
	// Tips:
	//	If the platform does not have one of the traits specified it can still be registered, but cannot
	//	be activated.
	//
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagContainer DependentPlatformTraits;

	//
	// If the current platform has any of these traits, then this config will not be actived.
	//
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagContainer ExcludedPlatformTraits;

	//
	// If true, then this input config will be activated when it's associated Game Feature is activated.
	// This is normally the desirable behavior
	//
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShouldActivateAutomatically{ true };

public:
	/** 
	 * Returns true if this config pair can be activated based on the current platform traits and settings. 
	 */
	bool CanBeActivated() const;

};