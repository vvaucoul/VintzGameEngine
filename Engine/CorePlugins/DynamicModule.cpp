/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DynamicModule.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 00:28:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 00:32:40 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// DynamicModule.cpp
#include "DynamicModule.h"
#include <iostream>	 // For error reporting
#include <stdexcept> // For exceptions (optional)

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace // Anonymous namespace for platform-specific helpers
{
#ifdef _WIN32
	void *LoadNativeLibrary(const char *path) {
		return LoadLibraryA(path);
	}
	void UnloadNativeLibrary(void *handle) {
		if (handle) FreeLibrary(static_cast<HMODULE>(handle));
	}
	void *GetNativeSymbol(void *handle, const char *symbolName) {
		return GetProcAddress(static_cast<HMODULE>(handle), symbolName);
	}
	std::string GetNativeError() {
		DWORD error	 = GetLastError();
		LPSTR buffer = nullptr;
		size_t size	 = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
									  NULL,
									  error,
									  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
									  (LPSTR)&buffer,
									  0,
									  NULL);
		std::string message(buffer, size);
		LocalFree(buffer);
		return message;
	}
#else
	void *LoadNativeLibrary(const char *path) {
		// RTLD_NOW: Resolve all symbols immediately.
		// RTLD_LOCAL: Symbols are not available for subsequently loaded libraries.
		return dlopen(path, RTLD_NOW | RTLD_LOCAL);
	}
	void UnloadNativeLibrary(void *handle) {
		if (handle) dlclose(handle);
	}
	void *GetNativeSymbol(void *handle, const char *symbolName) {
		return dlsym(handle, symbolName);
	}
	std::string GetNativeError() {
		const char *err = dlerror();
		return err ? std::string(err) : "Unknown dynamic linking error";
	}
#endif

	// Helper to load a symbol and report errors
	template <typename T>
	T LoadSymbol(void *handle, const char *name) {
		void *symbol = GetNativeSymbol(handle, name);
		if (!symbol) {
			std::cerr << "Error loading symbol '" << name << "': " << GetNativeError() << std::endl;
			return nullptr;
		}
		// C-style cast is necessary here for function pointers from void*
		return (T)symbol;
	}

} // namespace

DynamicModule::DynamicModule(const std::string &path) : m_Path(path), m_Handle(nullptr), m_LastWriteTime() {
	// Constructor can optionally attempt to load immediately, or Load() can be called later.
}

DynamicModule::~DynamicModule() {
	Unload(); // Ensure cleanup
}

bool DynamicModule::Load() {
	if (m_Handle) {
		Unload(); // Unload previous instance if any
	}

	// Check if file exists before trying to load
	if (!std::filesystem::exists(m_Path)) {
		std::cerr << "Error: Library file not found: " << m_Path << std::endl;
		return false;
	}

	m_Handle = LoadNativeLibrary(m_Path.c_str());
	if (!m_Handle) {
		std::cerr << "Error loading library '" << m_Path << "': " << GetNativeError() << std::endl;
		return false;
	}

	// Reset function pointers
	InitFunction	 = nullptr;
	UpdateFunction	 = nullptr;
	ShutdownFunction = nullptr;

	// Load standard plugin functions
	InitFunction	 = LoadSymbol<decltype(InitFunction)>(m_Handle, "Plugin_Init");
	UpdateFunction	 = LoadSymbol<decltype(UpdateFunction)>(m_Handle, "Plugin_Update");
	ShutdownFunction = LoadSymbol<decltype(ShutdownFunction)>(m_Handle, "Plugin_Shutdown");

	// Check if essential functions were loaded
	if (!InitFunction || !UpdateFunction || !ShutdownFunction) {
		std::cerr << "Error: Failed to load one or more required plugin functions from " << m_Path << std::endl;
		UnloadNativeLibrary(m_Handle); // Use helper directly to avoid calling ShutdownFunction
		m_Handle = nullptr;
		return false;
	}

	// Store the last write time on successful load
	try {
		m_LastWriteTime = std::filesystem::last_write_time(m_Path);
	} catch (const std::filesystem::filesystem_error &e) {
		std::cerr << "Warning: Could not get last write time for " << m_Path << ": " << e.what() << std::endl;
		// Proceeding without last write time means hot-reload won't work for this module
		m_LastWriteTime = {}; // Reset time
	}

	std::cout << "Successfully loaded plugin: " << m_Path << std::endl;
	return true;
}

void DynamicModule::Unload() {
	if (!m_Handle) return;

	// Call the plugin's shutdown function before unloading
	if (ShutdownFunction) {
		try {
			ShutdownFunction(); // Pass nullptr or engine context if needed
		} catch (const std::exception &e) {
			std::cerr << "Exception during Plugin_Shutdown for " << m_Path << ": " << e.what() << std::endl;
		} catch (...) {
			std::cerr << "Unknown exception during Plugin_Shutdown for " << m_Path << std::endl;
		}
	} else {
		std::cerr << "Warning: Shutdown function pointer was null for " << m_Path << " during unload." << std::endl;
	}

	UnloadNativeLibrary(m_Handle);
	m_Handle = nullptr;

	// Reset function pointers
	InitFunction	 = nullptr;
	UpdateFunction	 = nullptr;
	ShutdownFunction = nullptr;

	std::cout << "Unloaded plugin: " << m_Path << std::endl;
}

bool DynamicModule::IsLoaded() const {
	return m_Handle != nullptr;
}

const std::string &DynamicModule::GetPath() const {
	return m_Path;
}

std::filesystem::file_time_type DynamicModule::GetLastWriteTime() const {
	return m_LastWriteTime;
}
