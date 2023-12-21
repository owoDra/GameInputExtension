// Copyright (C) 2023 owoDra

#include "MappableConfigPair.h"

#include "CommonUISettings.h"
#include "ICommonUIModule.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MappableConfigPair)


bool FMappableConfigPair::CanBeActivated() const
{
	const auto& PlatformTraits{ ICommonUIModule::GetSettings().GetPlatformTraits() };

	// If the current platform does NOT have all the dependent traits, then don't activate it

	if (!DependentPlatformTraits.IsEmpty() && !PlatformTraits.HasAll(DependentPlatformTraits))
	{
		return false;
	}

	// If the platform has any of the excluded traits, then we shouldn't activate this config.

	if (!ExcludedPlatformTraits.IsEmpty() && PlatformTraits.HasAny(ExcludedPlatformTraits))
	{
		return false;
	}

	return true;
}
