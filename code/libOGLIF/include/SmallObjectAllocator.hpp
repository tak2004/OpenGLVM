#ifndef SMALLOBJECTALLOCATOR_HPP
#define SMALLOBJECTALLOCATOR_HPP

#include "libOGLIF.h"

namespace OGLIF {

class SmallObject
{
public:
    virtual ~SmallObject() {}
protected:
    SmallObject() {}
private:
    SmallObject(const SmallObject&);
    SmallObject& operator=(const SmallObject&);
};

class SmallObjectAllocator
{
public:
    SmallObjectAllocator(OGLIF_U32 ObjectSize, OGLIF_U32 PoolSize, 
                         void* (*Allocate)(OGLIF_SIZE Bytes),
                         void(*Deallocate)(void* MemoryPointer),
                         void* (*Reallocate)(void* MemoryPointer, OGLIF_SIZE NewSize));
    ~SmallObjectAllocator();

    SmallObject* Allocate();

    bool IsNoSpaceLeft();
private:
    OGLIF_U32 m_ObjectSize;
    OGLIF_U32 m_ObjectsPerPool;
    OGLIF_U32 m_FreeObjectCount;
    OGLIF_U32 m_PoolCount;
    OGLIF_U32 m_PoolSize;
    OGLIF_U8** m_Pools;
    void* (*m_Allocate)(OGLIF_SIZE);
    void(*m_Deallocate)(void*);
    void* (*m_Reallocate)(void*, OGLIF_SIZE);
};

}

#endif