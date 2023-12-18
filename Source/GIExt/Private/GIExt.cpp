// Copyright Epic Games, Inc. All Rights Reserved.

#include "GIExt.h"

#define LOCTEXT_NAMESPACE "FGIExtModule"

void FGIExtModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FGIExtModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGIExtModule, GIExt)