#include "SmallObjectAllocator.hpp"
#include <string.h>

namespace OGLIF { 

SmallObjectAllocator::SmallObjectAllocator(OGLIF_U32 ObjectSize, OGLIF_U32 PoolSize)
:m_FreeObjectCount(0)
,m_ObjectSize(ObjectSize)
,m_PoolCount(0)
,m_Pools(0)
,m_PoolSize(PoolSize)
,m_ObjectsPerPool(PoolSize/ObjectSize)
,m_MaxPools(0)
{

}

OGLIF_U8* SmallObjectAllocator::Allocate()
{
    OGLIF_U8* result = 0;
    if(m_FreeObjectCount > 0)
    {
        OGLIF_SIZE poolIndex = m_PoolCount*m_ObjectsPerPool / m_FreeObjectCount;
        OGLIF_SIZE byteOffsetInPool = (m_ObjectsPerPool - (m_FreeObjectCount % m_ObjectsPerPool))*m_ObjectSize;
        result = m_Pools[poolIndex] + byteOffsetInPool;
        --m_FreeObjectCount;
    }
    return result;
}

bool SmallObjectAllocator::AttachPool(OGLIF_U8* NewPool, OGLIF_U32 PoolSize)
{
    bool result = false;
    if(PoolSize == m_PoolSize && NewPool != 0 && m_Pools != 0 &&
       ReachedPoolLimit())
    {
        result = true;
        m_Pools[m_PoolCount] = reinterpret_cast<OGLIF_U8*>(NewPool);
        ++m_PoolCount;
    }
    return result;
}

OGLIF_U8* SmallObjectAllocator::ResizePoolList(OGLIF_U8* Memory, OGLIF_U32 Bytes)
{
    OGLIF_U8* result = 0;
    OGLIF_U32 newMaxPoolCount = Bytes / sizeof(void*);
    OGLIF_U32 oldObjectCount = m_PoolCount*m_ObjectsPerPool;
    if(Memory != 0 && newMaxPoolCount >= m_PoolCount)
    {
        m_MaxPools = newMaxPoolCount;
        memcpy(Memory, m_Pools, m_PoolCount*sizeof(void*));
        result = reinterpret_cast<OGLIF_U8*>(m_Pools);
        m_Pools = reinterpret_cast<OGLIF_U8**>(Memory);
    }
    return result;
}

bool SmallObjectAllocator::IsNoSpaceLeft()
{
    return m_FreeObjectCount == 0;
}

bool SmallObjectAllocator::ReachedPoolLimit()
{
    return m_MaxPools == m_PoolCount && IsNoSpaceLeft();
}

OGLIF_U32 SmallObjectAllocator::PoolSize() const
{
    return m_PoolSize;
}

OGLIF_U32 SmallObjectAllocator::PoolCount() const
{
    return m_PoolCount;
}

}