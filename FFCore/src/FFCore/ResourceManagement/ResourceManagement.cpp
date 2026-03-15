#include "FFCore/ResourceManagement/ResourceManagement.h"

#include <ranges>

using namespace FF;

void ResourceManager::UnloadAll()
{
    for (std::unordered_map<HString, std::shared_ptr<Resource>>& typeResources : _resources | std::views::values) 
    {
        for (std::shared_ptr<Resource>& resource : typeResources | std::views::values) 
        {
            if (resource && resource->IsLoaded()) 
            {
                resource->Unload();
            }
        }
    }
    
    _resources.clear();
    _refCounts.clear();
}

bool ResourceManager::ReadTextFile(HString fileName, std::string& outFile)
{
    std::ifstream fileStream(fileName.Get(), std::ios::binary);
    if (!fileStream)
    {
        LOG_ERROR("Error reading %s", fileName.Get())
        return false;
    }

    // get size
    fileStream.seekg(0, std::ios::end);
    const std::streamsize size = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);

    outFile.clear();
    if (size > 0)
    {
        outFile.resize(static_cast<size_t>(size));
        fileStream.read(outFile.data(), size);
        outFile.resize(static_cast<size_t>(fileStream.gcount()));
    }

    return true;
}