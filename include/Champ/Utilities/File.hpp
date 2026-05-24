#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <functional>

namespace Champ
{
	using FileLoadEvent = std::function<void(const void *data, uint64_t size, void *userData)>;

	class File
	{
	friend class ApplicationBase;
	public:
		static FileLoadEvent onLoad;
		static bool Exists(const std::string &filePath);
		static bool WriteAllText(const std::string &filePath, const std::string &text);
		static bool WriteAllBytes(const std::string &filePath, const void *data, size_t size);
		static bool Remove(const std::string &filePath);
		static std::string ReadAllText(const std::string &filePath);
		static std::vector<uint8_t> ReadAllBytes(const std::string &filePath);
		static std::vector<std::string> ReadAllLines(const std::string &filePath);
		static size_t GetSize(const std::string &filePath);
		static bool IsWithinDirectory(const std::string &filePath, const std::string &directoryPath);
		static std::string GetName(const std::string &filePath, bool withExtension);
		static std::string GetExtension(const std::string &filePath);
		static std::string GetDirectoryPath(const std::string &filePath);
		static bool ReadAllBytesAsync(const std::string &filePath, void *userData);
	private:
		static void NewFrame();
	};
}