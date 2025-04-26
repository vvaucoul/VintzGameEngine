#pragma once

#include <atomic>
#include <chrono>
#include <filesystem>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

/**
 * @brief A simple polling-based file watcher.
 * Monitors a list of files for changes in their last write time and triggers a callback.
 */
class FileWatcher {
public:
	using FileChangedCallback = std::function<void(const std::string & /*filePath*/)>;

	/**
	 * @brief Constructs a FileWatcher.
	 * @param callback The function to call when a file change is detected.
	 * @param pollInterval The interval at which to check files for changes.
	 */
	FileWatcher(FileChangedCallback callback, std::chrono::milliseconds pollInterval = std::chrono::milliseconds(1000));

	/**
	 * @brief Destructor. Stops the watcher thread.
	 */
	~FileWatcher();

	// Disable copy/move semantics
	FileWatcher(const FileWatcher &)			= delete;
	FileWatcher &operator=(const FileWatcher &) = delete;
	FileWatcher(FileWatcher &&)					= delete;
	FileWatcher &operator=(FileWatcher &&)		= delete;

	/**
	 * @brief Adds a file path to monitor.
	 * @param filePath The path to the file to watch.
	 * @return True if the path was added successfully (file exists), false otherwise.
	 */
	bool AddPath(const std::string &filePath);

	/**
	 * @brief Removes a file path from monitoring.
	 * @param filePath The path to stop watching.
	 */
	void RemovePath(const std::string &filePath);

	/**
	 * @brief Starts the monitoring thread.
	 */
	void Start();

	/**
	 * @brief Stops the monitoring thread.
	 */
	void Stop();

private:
	/**
	 * @brief The main loop executed by the watcher thread.
	 */
	void WatchLoop();

	FileChangedCallback m_Callback;
	std::chrono::milliseconds m_PollInterval;
	std::unordered_map<std::string, std::filesystem::file_time_type> m_PathsToWatch;
	std::mutex m_Mutex; // Protects m_PathsToWatch
	std::thread m_WatcherThread;
	std::atomic<bool> m_IsRunning;
};
