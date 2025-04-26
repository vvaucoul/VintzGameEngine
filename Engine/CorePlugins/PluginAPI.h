/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PluginAPI.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 00:26:33 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 00:27:52 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// PluginAPI.h
#pragma once

#ifdef _WIN32
#ifdef PLUGIN_EXPORTS
#define PLUGIN_API __declspec(dllexport)
#else
#define PLUGIN_API __declspec(dllimport)
#endif
#else
#define PLUGIN_API __attribute__((visibility("default")))
#endif

extern "C" {
/**
 * @brief Called immediately after the plugin is loaded by the engine.
 *
 * @param engineContext Pointer to engine context or API struct (implementation-defined).
 */
PLUGIN_API void Plugin_Init(void *engineContext);

/**
 * @brief Called every frame or at a regular interval by the engine.
 *
 * @param deltaTime Time elapsed since the last update (in seconds).
 */
PLUGIN_API void Plugin_Update(float deltaTime);

/**
 * @brief Called just before the plugin is unloaded, for cleanup.
 */
PLUGIN_API void Plugin_Shutdown();
}
