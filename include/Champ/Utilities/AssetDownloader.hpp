#pragma once

#include <string>
#include <functional>
#include <cstdint>

namespace Champ
{
    enum class FetchResult
    {
        Ok,
        Error
    };

    using FetchEvent = std::function<void(FetchResult result, uint16_t statusCode, const void* data, uint64_t size, void* userData)>;

    class AssetDownloader
    {
    public:
        static FetchEvent onFetch;
        static bool Fetch(const std::string &uri, void *userData);
    };
}