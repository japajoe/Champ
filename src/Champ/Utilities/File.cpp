
#include "File.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ranges>
#include <future>
#include "ConcurrentQueue.hpp"
#ifdef __EMSCRIPTEN__
#include <emscripten/fetch.h>
#include <cstring>
#endif

namespace fs = std::filesystem;

namespace Champ
{
	struct QueuedFile
	{
		uint8_t *data;
		uint64_t size;
		FileLoadCallback callback;
		void *userData;
	};

	static ConcurrentQueue<QueuedFile> gFileQueue;

    static const char DirectorySeparatorChar = '\\';
    static const char AltDirectorySeparatorChar = '/';
    static const char VolumeSeparatorChar = ':';

	bool File::Exists(const std::string &filePath)
	{
		return fs::exists(filePath) && fs::is_regular_file(filePath);
	}

	bool File::WriteAllText(const std::string &filePath, const std::string &text)
	{
        std::ofstream f(filePath, std::ios_base::out);
        if (f.is_open())
        {
            f << text;
            f.close();
			return true;
        }
		return false;
	}

	bool File::WriteAllBytes(const std::string &filePath, const void *data, size_t size)
	{
        std::ofstream f(filePath, std::ios::binary);

        if (f.is_open())
		{
            f.write(reinterpret_cast<const char*>(data), size);
            f.close();
			return true;
		}
		return false;
	}

	bool File::Remove(const std::string &filePath)
	{
		fs::path path(filePath);

		if (!fs::exists(path))
			return false;

		return fs::remove(path);
	}

	std::string File::ReadAllText(const std::string &filePath)
	{
		std::ifstream f(filePath);
		
        if (!f.is_open())
			throw std::runtime_error("Could not open file: " + filePath);

		std::ostringstream contentStream;
		contentStream << f.rdbuf();
        f.close();
		return contentStream.str();
	}

	std::vector<uint8_t> File::ReadAllBytes(const std::string &filePath)
	{
		std::ifstream f(filePath, std::ios::binary);
		
        if (!f.is_open()) 
			throw std::runtime_error("Could not open file: " + filePath);

		f.seekg(0, std::ios::end);
		std::streamsize fileSize = f.tellg();
		f.seekg(0, std::ios::beg);

		std::vector<uint8_t> buffer(fileSize);

		if (!f.read(reinterpret_cast<char*>(buffer.data()), fileSize))
			throw std::runtime_error("Error reading file: " + filePath);
        
        f.close();
		return buffer;
	}

	std::vector<std::string> File::ReadAllLines(const std::string &filePath)
	{
		std::ifstream f(filePath, std::ios::binary);
		
        if (!f.is_open()) 
			throw std::runtime_error("Unable to open file: " + filePath);

		std::vector<std::string> lines;
		std::string line;

		while (std::getline(f, line)) 
		{
			lines.push_back(line);
		}

		f.close();
		return lines;
	}

	size_t File::GetSize(const std::string &filePath)
	{
        if(!Exists(filePath))
            return 0;
		return fs::file_size(fs::path(filePath));
	}

    bool File::IsWithinDirectory(const std::string &filePath, const std::string &directoryPath)
    {
        if(!Exists(filePath))
            return false;

		try 
		{
			// Canonical resolves symlinks and ".." which is safer for security checks
			fs::path normRoot = fs::canonical(directoryPath);
			fs::path normChild = fs::canonical(filePath);

			// Mismatch finds the first point where the two paths diverge
			auto [rootIt, childIt] = std::ranges::mismatch(normRoot, normChild);

			// If we reached the end of the root path, then child starts with root
			return rootIt == normRoot.end();
		} 
		catch (const fs::filesystem_error&) 
		{
			return false;
		}
    }

    std::string File::GetName(const std::string &filePath, bool withExtension)
    {
        if (filePath.size() > 0)
        {
            size_t length = filePath.size();

            for (size_t i = length; --i > 0;)
            {
                char ch = filePath[i];

                if (ch == DirectorySeparatorChar || ch == AltDirectorySeparatorChar || ch == VolumeSeparatorChar)
                {
                    if(withExtension)
                    {
                        return filePath.substr(i + 1, length - i - 1);
                    }
                    else
                    {
                        // Find the position of the last dot (.)
                        size_t dotPos = filePath.find_last_of('.');
                        if (dotPos != std::string::npos && dotPos > i)
                            return filePath.substr(i + 1, dotPos - i - 1);
                        else
                            return filePath.substr(i + 1, length - i - 1);
                    }
                }
            }
        }
        return filePath;
    }

    std::string File::GetExtension(const std::string &filePath)
    {
        if (filePath.size() == 0)
            return "";

        size_t length = filePath.size();

        for (int i = length; --i > 0;)
        {
            char ch = filePath[i];

            if (ch == '.')
            {
                if (i != length - 1)
                    return filePath.substr(i, length - i);
                else
                    return "";
            }

            if (ch == DirectorySeparatorChar || ch == AltDirectorySeparatorChar || ch == VolumeSeparatorChar)
                break;
        }

        return "";
    }

	std::string File::GetDirectoryPath(const std::string &filePath)
	{
		if (filePath.empty()) 
			return "";

		std::filesystem::path p(filePath);
		
		// parent_path() extracts the directory part
		return p.parent_path().string();
	}

	static uint8_t *ReadFileBytes(const char *filePath, uint64_t *size) 
	{
		FILE *file = fopen(filePath, "rb");

		if (!file) 
			return nullptr;

		if (fseek(file, 0, SEEK_END) != 0) 
		{ 
			fclose(file); 
			return nullptr; 
		}

		long fileSize = ftell(file);
		
		if (fileSize < 0) 
		{ 
			fclose(file); 
			return nullptr; 
		}
		
		rewind(file);

		uint8_t *buf = (uint8_t*)malloc((size_t)fileSize);
		if (!buf) 
		{ 
			fclose(file); 
			return nullptr; 
		}

		size_t read = fread(buf, 1, (size_t)fileSize, file);
		
		if (read != (size_t)fileSize) 
		{
			free(buf);
			fclose(file);
			return nullptr;
		}

		fclose(file);
		if (size) 
			*size = (uint64_t)fileSize;
		return buf;
	}

#ifdef __EMSCRIPTEN__
    static void OnFetchSuccess(emscripten_fetch_t *fetch)
    {
		QueuedFile *file = static_cast<QueuedFile*>(fetch->userData);
        if(file->callback)
            file->callback(fetch->data, fetch->numBytes, file->userData);
        emscripten_fetch_close(fetch);
		delete file;
    }

    static void OnFetchFailure(emscripten_fetch_t *fetch)
    {
		QueuedFile *file = static_cast<QueuedFile*>(fetch->userData);
        if(file->callback)
            file->callback(nullptr, 0, file->userData);
        emscripten_fetch_close(fetch);
		delete file;
    }

	void File::ReadAllBytesAsync(const std::string &filePath, FileLoadCallback callback, void *userData)
	{
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        std::strcpy(attr.requestMethod, "GET");

		QueuedFile *file = new QueuedFile();
		file->callback = callback;
		file->data = nullptr;
		file->size = 0;
		file->userData = userData;
        
		attr.userData = file;

        // EMSCRIPTEN_FETCH_LOAD_TO_MEMORY stores data in the fetch->data buffer
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = OnFetchSuccess;
        attr.onerror = OnFetchFailure;

        emscripten_fetch_t *fetch = emscripten_fetch(&attr, filePath.c_str());

        if (fetch == nullptr)
        {
			if(callback)
				callback(nullptr, 0, userData);
        }
	}
#else
    void File::ReadAllBytesAsync(const std::string &filePath, FileLoadCallback callback, void *userData)
    {
        if(!File::Exists(filePath))
        {
			if(callback)
				callback(nullptr, 0, userData);
			return;
		}

		auto func = [&] () -> void {
			QueuedFile f;
			f.data = ReadFileBytes(filePath.c_str(), &f.size);
			if(f.size < 0)
				f.size = 0;
			f.callback = callback;
			f.userData = userData;
			gFileQueue.Enqueue(f);
		};
		
		auto result = std::async(std::launch::async, func);
    }
#endif

	void File::NewFrame()
	{
		if(gFileQueue.GetSize() > 0)
		{
			QueuedFile f;
			if(gFileQueue.TryDequeue(f))
			{
				if(f.callback)
					f.callback(f.data, f.size, f.userData);
				if(f.data)
					free(f.data);
			}
		}
	}
}