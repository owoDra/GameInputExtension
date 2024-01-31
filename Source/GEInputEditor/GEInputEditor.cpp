// Copyright (C) 2024 owoDra

#include "GEInputEditor.h"

#include "AssetTypeActions/AssetTypeActions_InputProcessorBlueprint.h"

IMPLEMENT_MODULE(FGEInputEditorModule, GEInputEditor)


void FGEInputEditorModule::StartupModule()
{
	RegisterAssetTypeActions();
}

void FGEInputEditorModule::ShutdownModule()
{
	UnregisterAssetTypeActions();
}


void FGEInputEditorModule::RegisterAssetTypeActionCategory()
{
	static const FName CategoryKey{ TEXT("Input") };
	static const FText CategoryDisplayName{ NSLOCTEXT("InputEditor","InputAssetsCategory", "Input") };

	Category = IAssetTools::Get().RegisterAdvancedAssetCategory(CategoryKey, CategoryDisplayName);
}

void FGEInputEditorModule::RegisterAssetTypeActions()
{
	RegisterAssetTypeActionCategory();

	RegisterAsset<FAssetTypeActions_InputProcessorBlueprint>(RegisteredAssetTypeActions);
}

void FGEInputEditorModule::UnregisterAssetTypeActions()
{
	UnregisterAssets(RegisteredAssetTypeActions);
}

void FGEInputEditorModule::UnregisterAssets(TArray<TSharedPtr<FAssetTypeActions_Base>>& RegisteredAssets)
{
	const auto* AssetToolsPtr{ FModuleManager::GetModulePtr<FAssetToolsModule>(NAME_AssetToolsModule) };
	if (!AssetToolsPtr)
	{
		return;
	}

	auto& AssetTools{ AssetToolsPtr->Get() };
	for (auto& AssetTypeActionIt : RegisteredAssets)
	{
		if (AssetTypeActionIt.IsValid())
		{
			AssetTools.UnregisterAssetTypeActions(AssetTypeActionIt.ToSharedRef());
		}
	}
}
