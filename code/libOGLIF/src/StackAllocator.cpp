#include "StackAllocator.hpp"
#include <string.h>

namespace OGLIF { 

StackAllocator::StackAllocator(OGLIF_U8* Arena, OGLIF_SIZE ArenaSizeInBytes)
:m_Start(Arena)
,m_End(Arena+ArenaSizeInBytes)
,m_Current(Arena)
{

}

OGLIF_U8* StackAllocator::Resize(OGLIF_U8* Arena, OGLIF_SIZE NewArenaSizeInBytes)
{
    OGLIF_U8* oldArena = 0;
    if(Arena != 0 && NewArenaSizeInBytes > (m_Current-m_Start))
    {
        OGLIF_SIZE byteOffset = m_Current - m_Start;
        oldArena = m_Start;
        memcpy(Arena, oldArena, byteOffset);
        m_Current = Arena + byteOffset;
        m_Start = Arena;
        m_End = m_Start + NewArenaSizeInBytes;
    }
    return oldArena;
}

OGLIF_U8* StackAllocator::Push(OGLIF_U32 Bytes)
{
    OGLIF_U8* result = 0;
    Bytes += sizeof(OGLIF_U32);

    if(m_Current + Bytes < m_End)
    {
        result = m_Current;
        m_Current += Bytes;
        *reinterpret_cast<OGLIF_U32*>(m_Current-sizeof(OGLIF_U32)) = Bytes;
    }
    return result;
}

OGLIF_U8* StackAllocator::Pop(OGLIF_U8* Data)
{
    OGLIF_U8* result = 0;
    if(Data != 0 || m_Current > m_Start)
    {
        OGLIF_U32 offset = *reinterpret_cast<OGLIF_U32*>(m_Current - sizeof(OGLIF_U32));
        result = m_Current - offset;
        if(result == Data)
        {
            m_Current = result;
        }
    }
    return result;
}

OGLIF_SIZE StackAllocator::ArenaSizeInBytes() const
{
    return m_End - m_Start;
}

}