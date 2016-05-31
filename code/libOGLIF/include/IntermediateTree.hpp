#ifndef INTERMEDIATETREE_HPP
#define INTERMEDIATETREE_HPP

#include <libOGLIF.h>
#include <SmallObjectAllocator.hpp>
#include <StackAllocator.hpp>

namespace OGLIF {

struct StateNode;
struct VariableNode;
struct CommandNode;
struct SymbolNode;

enum class ErrorCode
{
    Ok,
    OutOfMemory,
    InvalidParameter,

};

class IntermediateTree
{
public:
    IntermediateTree(void* Scratchpad, OGLIF_SIZE Size, Allocator Allocate, 
                     Deallocator Deallocate, Reallocator Reallocate);

    StateNode* AddState(const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes);

    StateNode* GetState(OGLIF_U32 Id);

    bool RemoveState(OGLIF_U32 Id);

    bool ContainsState(StateNode* State);

    VariableNode* AddVariable(const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes,
                     OGLIF_TYPE Type, const OGLIF_U8* Value, OGLIF_U32 ValueSizeInBytes);

    bool ContainsVariable(VariableNode* Variable);

    VariableNode* GetVariable(OGLIF_U32 Id);

    bool RemoveVariable(OGLIF_U32 Id);

    CommandNode* AddCommand(StateNode* State, OGLIF_U16 CommandId, 
                            const OGLIF_PARAMETER* Parameters);

    void Build();
    bool NeedRebuild();

    OGLIF_I32 ReadByteCode(OGLIF_U8* Data, OGLIF_U16 DataSize);

    Allocator GetAllocator()const;
    Deallocator GetDeallocator()const;
    Reallocator GetReallocator()const;
private:
    void* m_Scratchpad;
    Allocator m_Allocate;
    Deallocator m_Deallocate;
    Reallocator m_Reallocate;
    SmallObjectAllocator m_StatePool;
    SmallObjectAllocator m_VariablePool;
    SmallObjectAllocator m_CommandSymbolPool;
    StateNode* m_FirstState;
    StateNode* m_LastState;
    VariableNode* m_FirstVariable;
    VariableNode* m_LastVariable;
    SymbolNode* m_FirstSymbol;
    SymbolNode* m_LastSymbol;
    struct
    {
        StateNode* CurrentState;
        VariableNode* CurrentVariable;
    } m_BuildCursor;
    bool m_Rebuild;
};
static_assert(sizeof(IntermediateTree) <= 4096, "IntermediateTree must fit into the Scratchpad.");

}

#endif