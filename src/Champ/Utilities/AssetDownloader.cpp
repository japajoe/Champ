#include <AssetDownloader.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten/fetch.h>
#include <cstring>
#else
#include "File.hpp"
#endif

namespace Champ
{
    FetchEvent AssetDownloader::onFetch = nullptr;
#ifdef __EMSCRIPTEN__

    static void OnFetchSuccess(emscripten_fetch_t *fetch)
    {
        if(AssetDownloader::onFetch)
            AssetDownloader::onFetch(FetchResult::Ok, fetch->status, fetch->data, fetch->numBytes, fetch->userData);
        emscripten_fetch_close(fetch);
    }

    static void OnFetchFailure(emscripten_fetch_t *fetch)
    {
        if(AssetDownloader::onFetch)
            AssetDownloader::onFetch(FetchResult::Ok, fetch->status, nullptr, 0, fetch->userData);
        emscripten_fetch_close(fetch);
    }

    bool AssetDownloader::Fetch(const std::string &uri, void *userData)
    {
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        std::strcpy(attr.requestMethod, "GET");

        attr.userData = userData;

        // EMSCRIPTEN_FETCH_LOAD_TO_MEMORY stores data in the fetch->data buffer
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = OnFetchSuccess;
        attr.onerror = OnFetchFailure;

        emscripten_fetch_t *fetch = emscripten_fetch(&attr, uri.c_str());

        if (fetch == nullptr)
        {
            return false;
        }

        return true;
    }
#else
    bool AssetDownloader::Fetch(const std::string &uri, void *userData)
    {
        if(!File::Exists(uri))
            return false;
        auto data = File::ReadAllBytes(uri);
        if(AssetDownloader::onFetch)
            AssetDownloader::onFetch(FetchResult::Ok, 200, data.data(), data.size(), userData);
        return true;
    }
#endif
}