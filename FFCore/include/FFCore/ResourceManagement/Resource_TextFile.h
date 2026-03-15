#pragma once
#include "ResourceManagement.h"

namespace FF
{
    class Resource_TextFile : public Resource 
    {
    public:
        Resource_TextFile(HString id)
            : Resource(id) {}
        ~Resource_TextFile() override;
        
        bool DoLoad() override;
        bool DoUnload() override;
        
        std::string_view GetContent() const { return _contents; }
        
    private:
        std::string _contents;
    };
}
