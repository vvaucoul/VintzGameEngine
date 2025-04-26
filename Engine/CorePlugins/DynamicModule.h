/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DynamicModule.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 00:28:00 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 00:32:40 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once // Use #pragma once for include guard

#include "PluginAPI.h" // Include the API definition
#include <chrono>	   // For time points
#include <filesystem>  // For filesystem operations
#include <string>

/**
 * @brief Manages a dynamically loaded shared library (.so or .dll) representing a plugin.
 * Provides functionality to load, unload, and access exported plugin functions.
 */
class DynamicModule {
public:
	/**
	 * @brief Constructs a DynamicModule instance for a given library path.
	 * @param path Path to the shared library file.
	 */
	DynamicModule(const std::string &path);

	/**
	 * @brief Destructor. Ensures the module is unloaded.
	 */
	~DynamicModule();

	// Disable copy/move semantics for simplicity with raw handles
	DynamicModule(const DynamicModule &)			= delete;
	DynamicModule &operator=(const DynamicModule &) = delete;
	DynamicModule(DynamicModule &&)					= delete;
	DynamicModule &operator=(DynamicModule &&)		= delete;

	/**
	 * @brief Loads the shared library and resolves the standard plugin function pointers.
	 * @return True if loading and symbol resolution were successful, false otherwise.
	 */
	bool Load();

	/**
	 * @brief Unloads the shared library, calling the plugin's shutdown function first.
	 */
	void Unload();

	/**
	 * @brief Checks if the module is currently loaded.
	 * @return True if the module handle is valid, false otherwise.
	 */
	bool IsLoaded() const;

	/**
	 * @brief Gets the path to the shared library file.
	 * @return const reference to the path string.
	 */
	const std::string &GetPath() const;

	/**
	 * @brief Gets the last recorded write time of the library file when it was loaded.
	 * @return std::filesystem::file_time_type representing the last write time.
	 */
	std::filesystem::file_time_type GetLastWriteTime() const;

	// Pointers to the standard plugin functions
	decltype(&Plugin_Init) InitFunction			= nullptr;
	decltype(&Plugin_Update) UpdateFunction		= nullptr;
	decltype(&Plugin_Shutdown) ShutdownFunction = nullptr;

private:
	std::string m_Path;
	void *m_Handle = nullptr;
	std::filesystem::file_time_type m_LastWriteTime;
};
