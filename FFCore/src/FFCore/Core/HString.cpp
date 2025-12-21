#include "FFCore/Core/HString.h"

namespace FF
{
    char HString::hashTable[hashTableSize][hashTableMaxStringLength] = { {'\0'} };
    HString HString::None = HString();
}