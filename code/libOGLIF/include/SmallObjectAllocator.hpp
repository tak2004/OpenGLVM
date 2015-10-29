#ifndef SMALLOBJECTALLOCATOR_HPP
#define SMALLOBJECTALLOCATOR_HPP

#include "libOGLIF.h"

namespace OGLIF {

class SmallObjectAllocator
{
public:
    SmallObjectAllocator(OGLIF_U32 ObjectSize, OGLIF_U32 PoolSize);

    OGLIF_U8* Allocate();

    bool AttachPool(OGLIF_U8* NewPool, OGLIF_U32 PoolSize);
    OGLIF_U8* ResizePoolList(OGLIF_U8* Memory, OGLIF_U32 Bytes);

    bool IsNoSpaceLeft();
    bool ReachedPoolLimit();

    OGLIF_U32 PoolSize()const;
    OGLIF_U32 PoolCount()const;
private:
    OGLIF_U32 m_ObjectSize;
    OGLIF_U32 m_ObjectsPerPool;
    OGLIF_U32 m_FreeObjectCount;
    OGLIF_U32 m_PoolCount;
    OGLIF_U32 m_PoolSize;
    OGLIF_U32 m_MaxPools;
    OGLIF_U8** m_Pools;
};

}

#endif