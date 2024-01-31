// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/Blueprint.h"

#include "InputProcessorBlueprint.generated.h"

/**
 * Blueprint class used to make input processes on the editor
 * 
 * Tips:
 *	The input processor factory should pick this for you automatically
 */
UCLASS(BlueprintType)
class GEINPUT_API UInputProcessorBlueprint : public UBlueprint
{
	GENERATED_BODY()
public:
	UInputProcessorBlueprint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	////////////////////////////////////////////////////////////
	// Editor Only
public:
#if WITH_EDITOR
	virtual bool SupportedByDefaultBlueprintFactory() const override { return false; }
#endif

};
