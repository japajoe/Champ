
#include "File.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ranges>

namespace fs = std::filesystem;

namespace Champ
{
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
}