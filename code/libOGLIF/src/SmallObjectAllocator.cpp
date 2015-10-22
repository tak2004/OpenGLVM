#include "SmallObjectAllocator.hpp"
#include <string.h>

namespace OGLIF { 

SmallObjectAllocator::SmallObjectAllocator(OGLIF_U32 ObjectSize, OGLIF_U32 PoolSize,
                                           void* (*Allocate)(OGLIF_SIZE Bytes),
                                           void(*Deallocate)(void* MemoryPointer),
                                           void* (*Reallocate)(void* MemoryPointer, OGLIF_SIZE NewSize))
:m_FreeObjectCount(0)
,m_ObjectSize(ObjectSize)
,m_PoolCount(0)
,m_Pools(0)
,m_PoolSize(PoolSize)
,m_ObjectsPerPool(PoolSize/ObjectSize)
,m_Allocate(Allocate)
,m_Deallocate(Deallocate)
,m_Reallocate(Reallocate)
{

}

SmallObjectAllocator::~SmallObjectAllocator()
{
    if(m_Pools)
    {
        for (OGLIF_SIZE i = 0; i < m_PoolCount; ++i)
        {
            for (OGLIF_SIZE j = 0; j < m_ObjectsPerPool;++j)
            {
                reinterpret_cast<SmallObject*>(m_Pools[i]+m_ObjectSize*j)->~SmallObject();
            }
            m_Deallocate(m_Pools[i]);
        }
        m_Deallocate(m_Pools);
        m_Pools = 0;
    }
}

SmallObject* SmallObjectAllocator::Allocate()
{
    SmallObject* result = 0;
    if(m_FreeObjectCount > 0)
    {
        OGLIF_SIZE poolIndex = m_PoolCount*m_ObjectsPerPool / m_FreeObjectCount;
        OGLIF_SIZE byteOffsetInPool = (m_ObjectsPerPool - (m_FreeObjectCount % m_ObjectsPerPool))*m_ObjectSize;
        result = reinterpret_cast<SmallObject*>(m_Pools[poolIndex] + byteOffsetInPool);
        --m_FreeObjectCount;
    }
    else
    {
        // list isn't initialized yet
        if(m_PoolCount == 0)
        {
            m_Pools = reinterpret_cast<OGLIF_U8**>(m_Allocate(4096));
        }

        // reached limit of pointer list, resize
        if(m_PoolCount % (4096 / sizeof(void*)) == 0)
        {
            OGLIF_SIZE bytes = (m_PoolCount / (4096 / sizeof(void*)) + 1) * 4096;
            // if reallocate is available then use it
            if(m_Reallocate)
            {
                m_Pools = reinterpret_cast<OGLIF_U8**>(m_Reallocate(m_Pools, bytes));
            }
            else
            {// fall back
                void* p = m_Allocate(bytes);
                memcpy(p,m_Pools, bytes-4096);
                m_Deallocate(m_Pools);
                m_Pools = reinterpret_cast<OGLIF_U8**>(p);
            }
        }

        // create a new page an object
        m_Pools[m_PoolCount] = reinterpret_cast<OGLIF_U8*>(m_Allocate(m_PoolSize));
        m_FreeObjectCount += m_ObjectsPerPool;
        result = reinterpret_cast<SmallObject*>(m_Pools[m_PoolCount]);
        --m_FreeObjectCount;
        ++m_PoolCount;
    }
    return result;
}

}