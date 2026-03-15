#include "FFCore/ResourceManagement/Resource_TextFile.h"

using namespace FF;

Resource_TextFile::~Resource_TextFile()
{
    Unload();
}

bool Resource_TextFile::DoLoad()
{
    char buffer[1024];
    sprintf_s(buffer, 1024, "%s%s.txt", ResourcePaths::TextFilePath, GetId().Get());
    return ResourceManager::ReadTextFile(buffer, _contents);
}

bool Resource_TextFile::DoUnload()
{
    return true;
}