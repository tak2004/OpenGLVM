#include "Nodes.hpp"
#include <string.h>

namespace OGLIF {

void StateNode::SetState(const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes)
{
    Id = SymbolNode::HashFunction(Name, NameSizeInBytes);
    Symbol = 0;
    Next = 0;
    FirstCommand = 0;
    LastCommand = 0;
}

void StateNode::AddCommand(CommandNode* Command)
{
    if(FirstCommand == 0)
    {
        FirstCommand = Command;
        LastCommand = Command;
    }
    else
    {
        LastCommand->Next = Command;
        LastCommand = Command;
    }
}

void CommandNode::SetCommand(OGLIF_U16 Id)
{
    Next = 0;
    FunctionId = Id;
    for(OGLIF_SIZE i = 0; i < OGLIF::MaxParameterCount::Value; ++i)
    {
        Parameters[i] = 0;
    }
}

bool CommandNode::IsValid(OGLIF_U16 CommandId, OGLIF_U8 ParameterCount, 
                          const OGLIF_PARAMETER* Parameters)
{
    return true;
}

OGLIF_SIZE CommandNode::GetParameterCount() const
{
    return 0;
}

void VariableNode::SetVariable(const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes,
    OGLIF_TYPE Type_, const OGLIF_U8* Value_, OGLIF_U32 ValueSizeInBytes_)
{
    Symbol = 0;
    Next = 0;
    Type = Type_;
    Id = SymbolNode::HashFunction(Name, NameSizeInBytes);
}

OGLIF_U32 SymbolNode::HashFunction(const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes)
{
    OGLIF_U32 hash = 2166136261u;
    for(OGLIF_SIZE i = 0; i < NameSizeInBytes; ++i)
    {
        hash ^= *Name++;
        hash *= 16777619u;
    }
    return hash;
}

}