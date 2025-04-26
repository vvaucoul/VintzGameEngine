/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TestPlugin.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 01:01:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 01:01:04 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Engine/CorePlugins/PluginAPI.h" // Ajustez le chemin si nécessaire
#include <iostream>

namespace {
	// State local to this plugin instance
	int updateCounter = 0;
}

/**
 * @brief Called immediately after the plugin is loaded by the engine.
 */
PLUGIN_API void Plugin_Init(void * /*engineContext*/) {
	std::cout << "[TestPlugin] Initialized!" << std::endl;
	updateCounter = 0; // Reset counter on init/reload
}

/**
 * @brief Called every frame or at a regular interval by the engine.
 */
PLUGIN_API void Plugin_Update(float deltaTime) {
	updateCounter++;
	// Example: Modify this line during runtime and recompile the plugin
	std::cout << "[TestPlugin] Update! Count: " << updateCounter << ", DeltaTime: " << deltaTime << std::endl;
}

/**
 * @brief Called just before the plugin is unloaded, for cleanup.
 */
PLUGIN_API void Plugin_Shutdown() {
	std::cout << "[TestPlugin] Shutdown!" << std::endl;
}
