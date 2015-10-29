#ifndef STACKALLOCATOR_HPP
#define STACKALLOCATOR_HPP

#include "libOGLIF.h"

namespace OGLIF {

class StackAllocator
{
public:
    StackAllocator(OGLIF_U8* Arena, OGLIF_SIZE ArenaSizeInBytes);

    OGLIF_U8* Resize(OGLIF_U8* Arena, OGLIF_SIZE ArenaSizeInBytes);

    // Return 0 if no space is left else the allocated memory.
    OGLIF_U8* Push(OGLIF_U32 Bytes);
    // Return 0 if empty Data is 0 else the expected pointer.
    OGLIF_U8* Pop(OGLIF_U8* Data);
    // Return the size in bytes.
    OGLIF_SIZE ArenaSizeInBytes()const;
private:
    OGLIF_U8* m_Start;
    OGLIF_U8* m_End;
    OGLIF_U8* m_Current;
};

}

#endif