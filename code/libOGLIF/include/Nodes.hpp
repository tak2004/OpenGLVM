#ifndef NODES_HPP
#define NODSE_HPP

#include "libOGLIF.h"
#include "OpenGLTraits.hpp"

namespace OGLIF {

struct SymbolNode
{
    static OGLIF_U32 HashFunction(const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes);
    OGLIF_U32 Hash;
    OGLIF_U8  NameSizeInBytes;
    OGLIF_U8  Name[123];
};

struct CommandNode
{
    void SetCommand(OGLIF_U16 Id);

    static bool IsValid(OGLIF_U16 CommandId, OGLIF_U8 ParameterCount,
                        const OGLIF_PARAMETER* Parameters);

    CommandNode* Next;
    OGLIF_U32 Parameters[OGLIF::MaxParameterCount::Value];
    OGLIF_U16 FunctionId;

    OGLIF_SIZE GetParameterCount()const;
};

struct StateNode
{
    void SetState(const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes);
    void AddCommand(CommandNode* Command);

    StateNode* Next;
    SymbolNode* Symbol;
    CommandNode* FirstCommand;
    CommandNode* LastCommand;
    OGLIF_U32 Id;
};

struct VariableNode
{
    void SetVariable(const OGLIF_U8* Name_, OGLIF_U32 NameSizeInBytes_, 
        OGLIF_TYPE Type_, const OGLIF_U8* Value_, OGLIF_U32 ValueSizeInBytes_);

    VariableNode* Next;
    SymbolNode* Symbol;
    OGLIF_U8 Value[OGLIF::MaxParameterSize::Value];
    OGLIF_U32 Id;
    OGLIF_TYPE Type;
    OGLIF_U8 Amount;
};

}

#endif