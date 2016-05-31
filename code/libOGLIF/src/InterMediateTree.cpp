#include "IntermediateTree.hpp"
#include "Nodes.hpp"
#include "SmallObjectAllocator.hpp"
#include <new>

namespace OGLIF {

IntermediateTree::IntermediateTree(void* Scratchpad, OGLIF_SIZE Size, Allocator Allocate,
                                   Deallocator Deallocate, Reallocator Reallocate)
:m_Scratchpad(Scratchpad)
,m_Allocate(Allocate)
,m_Deallocate(Deallocate)
,m_Reallocate(Reallocate)
,m_FirstState(0)
,m_LastState(0)
,m_FirstVariable(0)
,m_LastVariable(0)
,m_FirstSymbol(0)
,m_LastSymbol(0)
,m_Rebuild(true)
,m_CommandSymbolPool(sizeof(SymbolNode), 4096)
,m_VariablePool(sizeof(VariableNode), 4096)
,m_StatePool(sizeof(StateNode), 4096)
{
    m_BuildCursor = {0};
}

StateNode* IntermediateTree::AddState(const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes)
{
    StateNode* result = 0;

    if(m_StatePool.ReachedPoolLimit())
    {
        OGLIF_SIZE bytes = m_StatePool.PoolSize() * m_StatePool.PoolCount() * 2;
        void* memory = m_Allocate(bytes);
        OGLIF_U8* oldMemory = m_StatePool.ResizePoolList(reinterpret_cast<OGLIF_U8*>(memory), bytes);
        m_Deallocate(oldMemory);
    }

    if(m_StatePool.IsNoSpaceLeft())
    {
        void* memory = m_Allocate(m_StatePool.PoolSize());
        m_StatePool.AttachPool(reinterpret_cast<OGLIF_U8*>(memory), m_StatePool.PoolSize());
    }

    result = reinterpret_cast<StateNode*>(m_StatePool.Allocate());
    result->SetState(Name, NameSizeInBytes);

    if(m_FirstState == 0)
    {
        m_FirstState = result;
        m_LastState = result;
    }
    else
    {
        m_LastState->Next = result;
        m_LastState = result;
    }

    m_Rebuild = true;
    return result;
}

StateNode* IntermediateTree::GetState(OGLIF_U32 Id)
{
    StateNode* result = 0;
    return result;
}

bool IntermediateTree::RemoveState(OGLIF_U32 Id)
{
    return false;
}

bool IntermediateTree::ContainsState(StateNode* State)
{
    return false;
}

VariableNode* IntermediateTree::AddVariable(const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes,
    OGLIF_TYPE Type, const OGLIF_U8* Value, OGLIF_U32 ValueSizeInBytes)
{
    VariableNode* result = 0;

    if(m_VariablePool.ReachedPoolLimit())
    {
        OGLIF_SIZE bytes = m_VariablePool.PoolSize() * m_VariablePool.PoolCount() * 2;
        void* memory = m_Allocate(bytes);
        OGLIF_U8* oldMemory = m_VariablePool.ResizePoolList(reinterpret_cast<OGLIF_U8*>(memory), bytes);
        m_Deallocate(oldMemory);
    }

    if(m_VariablePool.IsNoSpaceLeft())
    {
        void* memory = m_Allocate(m_VariablePool.PoolSize());
        m_VariablePool.AttachPool(reinterpret_cast<OGLIF_U8*>(memory), m_VariablePool.PoolSize());
    }

    result = reinterpret_cast<VariableNode*>(m_VariablePool.Allocate());
    result->SetVariable(Name, NameSizeInBytes, Type, Value, ValueSizeInBytes);
        
    if(m_FirstVariable == 0)
    {
        m_FirstVariable = result;
        m_LastVariable = result;
    }
    else
    {
        m_LastVariable->Next = result;
        m_LastVariable = result;
    }

    m_Rebuild = true;
    return result;
}

bool IntermediateTree::ContainsVariable(VariableNode* Variable)
{
    return false;
}

VariableNode* IntermediateTree::GetVariable(OGLIF_U32 Id)
{
    return 0;
}

bool IntermediateTree::RemoveVariable(OGLIF_U32 Id)
{
    return false;
}

CommandNode* IntermediateTree::AddCommand(StateNode* State, OGLIF_U16 CommandId,
    const OGLIF_PARAMETER* Parameters)
{
    CommandNode* result = 0;

    if(m_CommandSymbolPool.ReachedPoolLimit())
    {
        OGLIF_SIZE bytes = m_CommandSymbolPool.PoolSize() * m_CommandSymbolPool.PoolCount() * 2;
        void* memory = m_Allocate(bytes);
        OGLIF_U8* oldMemory = m_CommandSymbolPool.ResizePoolList(reinterpret_cast<OGLIF_U8*>(memory), bytes);
        m_Deallocate(oldMemory);
    }

    if(m_CommandSymbolPool.IsNoSpaceLeft())
    {
        void* memory = m_Allocate(m_CommandSymbolPool.PoolSize());
        m_CommandSymbolPool.AttachPool(reinterpret_cast<OGLIF_U8*>(memory), m_CommandSymbolPool.PoolSize());
    }

    result = reinterpret_cast<CommandNode*>(m_CommandSymbolPool.Allocate());
    result->SetCommand(CommandId);
    for (OGLIF_SIZE i = 0; i < result->GetParameterCount(); ++i)
    {
        OGLIF_U32 hash = 0;
        if(Parameters[i].Variable != OGLIF_INVALIDHANDLE)
        {
            auto* variable = reinterpret_cast<VariableNode*>(Parameters[i].Variable);
            hash = variable->Hash;
        }
        else
        {
            const OGLIF_U8 name[] = "möp";
            auto* variable = AddVariable(name, sizeof(name), Parameters[i].Type, Parameters[i].Value, Parameters[i].ValueSize);
            hash = variable->Id;
        }
        result->Parameters[i] = hash;
    }
    State->AddCommand(result);

    m_Rebuild = true;
    return result;
}

void IntermediateTree::Build()
{
    m_BuildCursor.CurrentState = m_FirstState;
    m_BuildCursor.CurrentVariable = m_FirstVariable;
    m_Rebuild = false;
}

bool IntermediateTree::NeedRebuild()
{
    return m_Rebuild;
}

OGLIF_I32 IntermediateTree::ReadByteCode(OGLIF_U8* Data, OGLIF_U16 DataSize)
{
    OGLIF_I32 result = -1;
    if(m_BuildCursor.CurrentState != 0)
    {

    }
    else if(m_BuildCursor.CurrentVariable != 0)
    {

    }
    return result;
}

Allocator IntermediateTree::GetAllocator() const
{
    return m_Allocate;
}

Deallocator IntermediateTree::GetDeallocator() const
{
    return m_Deallocate;
}

Reallocator IntermediateTree::GetReallocator() const
{
    return m_Reallocate;
}

}