// Copyright (C) 2024 owoDra

#pragma once

#include "PlayerMappableKeySettings.h"

#include "KeybindSettings.generated.h"


/**
 * Player Mappable Key settings are settings that are accessible per-action key mapping.
 * This is where you could place additional metadata that may be used by your settings UI,
 * input triggers, or other places where you want to know about a key setting.
 */
UCLASS()
class GEINPUT_API UKeybindSettings : public UPlayerMappableKeySettings
{
	GENERATED_BODY()

protected:
	//
	// The tooltip that should be associated with this action when displayed on the settings screen
	//
	UPROPERTY(EditAnywhere, Category = "Settings")
	FText Tooltip{ FText::GetEmpty() };

public:
	/**
	 * Returns the tooltip that should be displayed on the settings screen for this key
	 */
	const FText& GetTooltipText() const { return Tooltip; }

};
