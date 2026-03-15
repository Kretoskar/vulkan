#pragma once

#include <functional>
#include <queue>
#include <typeindex>
#include <unordered_map>

#include "FFCore/Core/HString.h"

namespace std
{
    class type_index;
}

namespace FF
{
    namespace ResourcePaths
    {
        inline const char* TextFilePath = "Z:/Dev/Personal/FFEngine/Resources/TextFiles/";
    }
    
    class Resource
    {
    public:
        explicit Resource(HString id) : _resourceId(id) {}
        virtual ~Resource() = default;

        // Core resource identity and state access methods
        HString GetId() const { return _resourceId; }
        bool IsLoaded() const { return _loaded; }

        // Virtual interface for resource-specific loading and unloading behavior
        bool Load() 
        {
            _loaded = DoLoad();
            return _loaded;
        }

        void Unload()
        {
            DoUnload();
            _loaded = false;
        }

    protected:
        virtual bool DoLoad() = 0;
        virtual bool DoUnload() = 0;
        
    private:
        HString _resourceId;     // Unique identifier for this resource within the system
        bool _loaded = false;    // Loading state flag for resource lifecycle management
    };
    
    class ResourceManager;
    
    template<typename T>
    class ResourceHandle
    {
    public:
        ResourceHandle() : _resourceManager(nullptr) {}
        ResourceHandle(HString id, ResourceManager* manager)
            : _resourceId(id), _resourceManager(manager) {}
        
        T* Get() const;
        bool IsValid() const;
        HString GetId() const; 
        
        T* operator->() const 
        {
            return Get();
        }

        T& operator*() const 
        {
            return *Get();
        }

        operator bool() const 
        {
            return IsValid();
        }
        
    private:
        HString _resourceId;
        ResourceManager* _resourceManager;
    };

    class ResourceManager
    {
    public:
        ResourceManager();
        ~ResourceManager();
        
        template<typename T>
        ResourceHandle<T> Load(HString resourceId);
        
        template<typename T>
        void LoadAsync(const std::string& resourceId, bool keep, std::function<void(ResourceHandle<T>)> callback);
        
        template<typename T>
        T* GetResource(HString resourceId);

        template<typename T>
        bool HasResource(HString resourceId);
        
        template<typename T>
        void Release(HString resourceId);
        
        void Start();
        void Stop();
        void UnloadAll();
        
        static bool ReadTextFile(HString fileName, std::string& outFile);
        
    private:
        void WorkerThread();
        
        // Two-level storage system: organize by type first, then by unique identifier
        // This approach enables type-safe resource access while maintaining efficient lookup
        std::unordered_map<std::type_index,
                           std::unordered_map<HString, std::shared_ptr<Resource>>> _resources;
        
        // Two-level reference counting system for automatic resource lifecycle management
        // First level maps resource type, second level maps resource IDs to their data
        struct ResourceData 
        {
            std::shared_ptr<Resource> resource;  // The actual resource
            i32 refCount;                        // Reference count for this resource
        };
        
        std::unordered_map<std::type_index,
                      std::unordered_map<HString, ResourceData>> _refCounts;
        
        std::thread _workerThread;
        std::queue<std::function<void()>> _taskQueue;
        std::mutex _queueMutex;
        std::condition_variable _condition;
        std::atomic_bool _running = false;
    };
    
    template <typename T>
    T* ResourceHandle<T>::Get() const
    {
        if (!_resourceManager)
        {
            return nullptr;
        }
        return _resourceManager->GetResource<T>(_resourceId);
    }

    template <typename T>
    bool ResourceHandle<T>::IsValid() const
    {
        return _resourceManager && _resourceManager->HasResource<T>(_resourceId);
    }

    template <typename T>
    HString ResourceHandle<T>::GetId() const
    {
        return _resourceId;
    }

    template <typename T>
    ResourceHandle<T> ResourceManager::Load(HString resourceId)
    {
        
        static_assert(std::is_base_of_v<Resource, T>, "T must derive from Resource");

        auto type = std::type_index(typeid(T));
        auto& typeResources = _resources[type];
        auto& typeRefCounts = _refCounts[type];

        // Try to find if exists
        auto it = typeResources.find(resourceId);
        if (it != typeResources.end()) 
        {
            auto rcIt = typeRefCounts.find(resourceId);
            if (rcIt != typeRefCounts.end()) 
            {
                rcIt->second.refCount++;
            }
            return ResourceHandle<T>(resourceId, this);
        }

        // Create new
        auto resource = std::make_shared<T>(resourceId);
        if (!resource->Load()) 
        {
            return ResourceHandle<T>();
        }

        typeResources[resourceId] = resource;
        typeRefCounts[resourceId] = ResourceData{resource, 1};

        return ResourceHandle<T>(resourceId, this);
    }
    
    template <typename T>
    void ResourceManager::LoadAsync(const std::string& resourceId, bool keep, std::function<void(ResourceHandle<T>)> callback)
    {
        std::lock_guard<std::mutex> lock(_queueMutex);
        _taskQueue.push([this, resourceId, callback, keep]() 
        {
            auto handle = Load<T>(resourceId);
            callback(handle);
            if (!keep)
            {
                Release<T>(resourceId);
            }
        });
        _condition.notify_one();
    }

    template <typename T>
    T* ResourceManager::GetResource(HString resourceId)
    {
        
        auto& typeResources = _resources[std::type_index(typeid(T))];
        auto it = typeResources.find(resourceId);

        if (it != typeResources.end()) 
        {
            return static_cast<T*>(it->second.get());
        }

        return nullptr;
    }

    template <typename T>
    bool ResourceManager::HasResource(HString resourceId)
    {
        auto resourceIt = _resources.find(std::type_index(typeid(T)));
        if (resourceIt == _resources.end()) 
        {
            return false;
        }
        return resourceIt->second.contains(resourceId);
    }

    template <typename T>
    void ResourceManager::Release(HString resourceId)
    {
        auto type = std::type_index(typeid(T));

        auto refTypeIt = _refCounts.find(type);
        if (refTypeIt == _refCounts.end()) return;

        auto rcIt = refTypeIt->second.find(resourceId);
        if (rcIt == refTypeIt->second.end()) return;

        rcIt->second.refCount--;

        if (rcIt->second.refCount <= 0)
        {
            if (rcIt->second.resource && rcIt->second.resource->IsLoaded()) 
            {
                rcIt->second.resource->Unload();
            }
            refTypeIt->second.erase(rcIt);

            auto resTypeIt = _resources.find(type);
            if (resTypeIt != _resources.end()) 
            {
                resTypeIt->second.erase(resourceId);
                if (resTypeIt->second.empty()) 
                {
                    _resources.erase(resTypeIt);
                }
            }

            if (refTypeIt->second.empty()) {
                _refCounts.erase(refTypeIt);
            }
        }
    }
}
