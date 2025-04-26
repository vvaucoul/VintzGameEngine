/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileWatcher.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 00:28:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 00:33:21 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileWatcher.h"
#include <iostream>	 // For potential logging
#include <stdexcept> // For invalid_argument

FileWatcher::FileWatcher(FileChangedCallback callback, std::chrono::milliseconds pollInterval) : m_Callback(std::move(callback)),
																								 m_PollInterval(pollInterval),
																								 m_IsRunning(false) {
	if (!m_Callback) {
		throw std::invalid_argument("FileWatcher callback cannot be null.");
	}
}

FileWatcher::~FileWatcher() {
	Stop();
}

bool FileWatcher::AddPath(const std::string &filePath) {
	std::filesystem::path pathObj(filePath);
	if (!std::filesystem::exists(pathObj) || !std::filesystem::is_regular_file(pathObj)) {
		std::cerr << "FileWatcher Error: Cannot watch non-existent or non-regular file: " << filePath << std::endl;
		return false;
	}

	try {
		auto lastWriteTime = std::filesystem::last_write_time(pathObj);
		std::lock_guard<std::mutex> lock(m_Mutex);
		// Only add if not already present, or update if it is (though update isn't strictly necessary here)
		m_PathsToWatch.insert_or_assign(filePath, lastWriteTime);
		std::cout << "FileWatcher: Started watching " << filePath << std::endl;
		return true;
	} catch (const std::filesystem::filesystem_error &e) {
		std::cerr << "FileWatcher Error: Could not get last write time for " << filePath << ": " << e.what() << std::endl;
		return false;
	}
}

void FileWatcher::RemovePath(const std::string &filePath) {
	std::lock_guard<std::mutex> lock(m_Mutex);
	if (m_PathsToWatch.erase(filePath)) {
		std::cout << "FileWatcher: Stopped watching " << filePath << std::endl;
	}
}

void FileWatcher::Start() {
	if (m_IsRunning.exchange(true)) {
		return; // Already running
	}

	m_WatcherThread = std::thread(&FileWatcher::WatchLoop, this);
	std::cout << "FileWatcher: Started monitoring thread." << std::endl;
}

void FileWatcher::Stop() {
	if (!m_IsRunning.exchange(false)) {
		return; // Already stopped
	}

	// No need to explicitly signal the thread other than setting m_IsRunning to false.
	// The sleep_for will eventually expire, or the loop condition will fail.

	if (m_WatcherThread.joinable()) {
		m_WatcherThread.join();
	}
	std::cout << "FileWatcher: Stopped monitoring thread." << std::endl;
}

void FileWatcher::WatchLoop() {
	while (m_IsRunning) {
		{ // Scope for lock guard
			std::lock_guard<std::mutex> lock(m_Mutex);
			for (auto it = m_PathsToWatch.begin(); it != m_PathsToWatch.end(); /* manual increment */) {
				const std::string &path	 = it->first;
				auto &lastKnownWriteTime = it->second;

				try {
					// Check existence first
					if (!std::filesystem::exists(path)) {
						// File was deleted, notify and remove
						std::cerr << "FileWatcher Warning: Watched file deleted: " << path << std::endl;
						// It's important to call the callback *before* erasing,
						// so the client knows which file triggered it, even if it's gone.
						m_Callback(path);
						it = m_PathsToWatch.erase(it); // Remove from map
						continue;					   // Continue to next file in the map
					}

					auto currentWriteTime = std::filesystem::last_write_time(path);

					// Compare the current write time with the last known write time
					if (currentWriteTime != lastKnownWriteTime) {
						lastKnownWriteTime = currentWriteTime; // Update the known time
						// File changed, trigger callback
						std::cout << "FileWatcher: Detected change in " << path << std::endl;
						m_Callback(path); // Call the registered callback function
					}
				} catch (const std::filesystem::filesystem_error &e) {
					// Handle potential errors during file access (e.g., permissions, file disappearing between checks)
					std::cerr << "FileWatcher Error accessing " << path << ": " << e.what() << std::endl;
					// Decide how to handle persistent errors. Maybe remove the path after N failures?
					// For now, just log the error and continue trying.
					// If the error was due to deletion, the exists() check next iteration should catch it.
				}
				++it; // Increment iterator only if the element was not erased
			}
		} // Mutex released here

		// Wait for the next poll interval before checking again
		// Check m_IsRunning periodically during sleep to allow faster shutdown
		auto wakeUpTime = std::chrono::steady_clock::now() + m_PollInterval;
		while (m_IsRunning && std::chrono::steady_clock::now() < wakeUpTime) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep in smaller chunks
		}
	}
}
