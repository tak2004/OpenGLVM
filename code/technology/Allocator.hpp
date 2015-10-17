#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <RFECS/Allocator.hpp>
#include <RadonFramework/Core/Types/UInt8.hpp>
#include <RadonFramework/Collections/AutoVector.hpp>

namespace Technology {

struct Allocator: public RFECS::Allocator
{
    virtual void* Allocate(RF_Type::Size Bytes, RF_Type::Size ByteAlignment)
    {
        RF_Mem::AutoPointerArray<RF_Type::UInt8> newBlock(Bytes);
        void* result = newBlock.Get();
        m_ResizablePool.PushBack(newBlock);
        return result;
    }

    virtual void Free(void* MemoryBlock)
    {
        for(RF_Type::Size i = 0; i < m_ResizablePool.Count(); ++i)
        {
            if(m_ResizablePool[i] == MemoryBlock)
            {
                m_ResizablePool.Erase(i);
                break;
            }
        }
    }
private:
    RF_Collect::AutoVector<RF_Type::UInt8> m_ResizablePool;
};

}

#endif // ALLOCATOR_HPP