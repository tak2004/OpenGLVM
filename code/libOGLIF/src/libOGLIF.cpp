#define OGLIF_BUILD
#include "libOGLIF.h"
#include "SmallObjectAllocator.hpp"
#include "Nodes.hpp"
#include "IntermediateTree.hpp"
#include <new>

// 
union Page
{
    OGLIF_U8 Bytes[4096];
    struct MappingType
    {
        Page* Next;
        OGLIF_U8 TreeOffset;
    } Mapping;
};

Page* firstPage = 0;

bool IsRightFormat(const OGLIF_U8* Data, OGLIF_U32 DataSize)
{
    bool result = false;
    auto* identifier = reinterpret_cast<const OGLIF_Identifier*>(Data);
    if(identifier->FourCC == OGLIF_FOURCC && identifier->Version == 1 &&
       DataSize >= sizeof(OGLIF_Identifier) + sizeof(OGLIF_Header))
    {
        result = true;
    }
    return result;
}

OGLIF_HANDLE OGLIFAPIENTRY ParseFromMemory(const OGLIF_U8* Data, OGLIF_U32 DataSize,
    Allocator Allocate, Deallocator Deallocate, Reallocator Reallocate, OGLIF_HANDLE* Destination)
{
    OGLIF_HANDLE result = OGLIF_OK;
    if(IsRightFormat(Data,DataSize))
    {
        result = CreateCodeTree(Allocate, Deallocate, Reallocate, Destination);
        if(result == OGLIF_OK)
        {
            Page* p = reinterpret_cast<Page*>(Destination);
            auto* tree = reinterpret_cast<OGLIF::IntermediateTree*>(&p->Mapping.TreeOffset);
        }
    }
    return result;
}

OGLIF_ERROR OGLIFAPIENTRY CreateCodeTree(Allocator Allocate, Deallocator Deallocate,
    Reallocator Reallocate, OGLIF_HANDLE* Destination)
{
    OGLIF_ERROR result = OGLIF_INVALIDPARAMETER;

    if(Allocate == 0 && Deallocate == 0 && Reallocate == 0)
    {
        Allocate = malloc;
        Deallocate = free;
        Reallocate = realloc;
    }

    if(Allocate != 0 && Deallocate != 0 && Destination != 0)
    {
        Page* p = reinterpret_cast<Page*>(Allocate(sizeof(Page)));
        if(p)
        {
            // add to link list
            Page** lastPage = &firstPage;
            while(*lastPage)
            {
                lastPage = &(*lastPage)->Mapping.Next;
            }
            *lastPage = p;
            p->Mapping.Next = 0;

            // init IntermediateTree
            void* scratchpad = reinterpret_cast<void*>(&p->Mapping.TreeOffset);
            new(scratchpad)OGLIF::IntermediateTree(scratchpad,
                sizeof(Page) - offsetof(struct Page::MappingType, TreeOffset), 
                Allocate, Deallocate, Reallocate);
                
            *Destination = reinterpret_cast<OGLIF_HANDLE>(p);
            result = OGLIF_OK;
        }
        else
        {
            result = OGLIF_OUTOFMEMORY;
        }
    }
    return result;
}

Page* GetPage(OGLIF_HANDLE CodeTree)
{
    Page* page = reinterpret_cast<Page*>(CodeTree);
    Page** lastPage = &firstPage;
    while(*lastPage != page && *lastPage != 0)
    {
        lastPage = &(*lastPage)->Mapping.Next;
    }
    return *lastPage;
}

OGLIF_ERROR OGLIFAPIENTRY FreeCodeTree(OGLIF_HANDLE CodeTree)
{
    OGLIF_ERROR result = OGLIF_OK;
    if(CodeTree != OGLIF_INVALIDHANDLE)
    {
        Page* page = GetPage(CodeTree);
        if(page != 0)
        {
            auto* tree = reinterpret_cast<OGLIF::IntermediateTree*>(&page->Mapping.TreeOffset);
            Deallocator deallocate = tree->GetDeallocator();
            // destroy the IntermediateTree
            tree->~IntermediateTree();
            
            // get page before and assign the next one to it
            Page** lastPage = &firstPage;
            while(*lastPage != page)
            {
                lastPage = &(*lastPage)->Mapping.Next;
            }
            *lastPage = page->Mapping.Next;

            // free memory
            deallocate(page);
        }
        else
        {
            result = OGLIF_INVALIDPARAMETER;
        }
    }
    return result;
}

OGLIF_ERROR OGLIFAPIENTRY AddState(OGLIF_HANDLE CodeTree, const OGLIF_U8* Name,
    OGLIF_U32 NameSizeInBytes, OGLIF_HANDLE* Destination)
{
    OGLIF_ERROR result = OGLIF_INVALIDPARAMETER;
    if(CodeTree != OGLIF_INVALIDHANDLE && Name != 0 && NameSizeInBytes > 0 && Destination != 0)
    {
        Page* page = GetPage(CodeTree);
        if(page != 0)
        {
            auto* tree = reinterpret_cast<OGLIF::IntermediateTree*>(&page->Mapping.TreeOffset);
            OGLIF_U32 stateId = OGLIF::SymbolNode::HashFunction(Name, NameSizeInBytes);
            if(tree->GetState(stateId) == 0)
            {
                *Destination = reinterpret_cast<OGLIF_HANDLE>(tree->AddState(Name, NameSizeInBytes));

                if(*Destination == OGLIF_INVALIDHANDLE)
                {
                    result = OGLIF_OUTOFMEMORY;
                }
                else
                {
                    result = OGLIF_OK;
                }
            }
            else
            {

            }
        }
        else
        {
            result = OGLIF_INVALIDPARAMETER;
        }
    }
    return result;
}

bool IsValidValue(OGLIF_TYPE Type, const OGLIF_U8* Value, OGLIF_U32 ValueSizeInBytes)
{
    return true;
}

OGLIF_ERROR OGLIFAPIENTRY AddVariable(OGLIF_HANDLE CodeTree, const OGLIF_U8* Name,
    OGLIF_U32 NameSizeInBytes, OGLIF_TYPE Type, const OGLIF_U8* Value, 
    OGLIF_U32 ValueSizeInBytes, OGLIF_HANDLE* Destination)
{
    OGLIF_ERROR result = OGLIF_INVALIDPARAMETER;
    if(CodeTree != OGLIF_INVALIDHANDLE && IsValidValue(Type, Value, ValueSizeInBytes) && Destination != 0)
    {
        Page* page = GetPage(CodeTree);
        if(page)
        {
            auto* tree = reinterpret_cast<OGLIF::IntermediateTree*>(&page->Mapping.TreeOffset);
            OGLIF_U32 variableId = OGLIF::SymbolNode::HashFunction(Name, NameSizeInBytes);
            if(tree->GetVariable(variableId))
            {
                *Destination = reinterpret_cast<OGLIF_HANDLE>(tree->AddVariable(Name, NameSizeInBytes,
                    Type, Value, ValueSizeInBytes));

                if(*Destination == OGLIF_INVALIDHANDLE)
                {
                    result = OGLIF_OUTOFMEMORY;
                }
                else
                {
                    result = OGLIF_OK;
                }
            }
        }
    }
    return result;
}

OGLIF_HANDLE OGLIFAPIENTRY AddCommand(OGLIF_HANDLE CodeTree, OGLIF_HANDLE State, OGLIF_U16 CommandId,
    OGLIF_U8 ParameterCount, const OGLIF_PARAMETER* Parameters)
{
    OGLIF_HANDLE result = OGLIF_INVALIDHANDLE;
    if(State != OGLIF_INVALIDHANDLE && CodeTree != OGLIF_INVALIDHANDLE &&
       OGLIF::CommandNode::IsValid(CommandId, ParameterCount, Parameters))
    {
        OGLIF::IntermediateTree* tree = reinterpret_cast<OGLIF::IntermediateTree*>(CodeTree);
        OGLIF::StateNode* state = reinterpret_cast<OGLIF::StateNode*>(State);
        result = reinterpret_cast<OGLIF_HANDLE>(tree->AddCommand(state, CommandId, Parameters));
    }
    return result;
}

OGLIF_I32 OGLIFAPIENTRY BuildByteCode(OGLIF_HANDLE Handle, OGLIF_U8* Data, OGLIF_U16 DataSize)
{
    OGLIF_I32 result = -1;
    if(DataSize > 0 && Handle != OGLIF_INVALIDHANDLE)
    {
        OGLIF::IntermediateTree* tree = reinterpret_cast<OGLIF::IntermediateTree*>(Handle);
        if(tree->NeedRebuild())
        {
            tree->Build();
        }
        result = tree->ReadByteCode(Data, DataSize);
    }
    return result;
}

OGLIF_ERROR OGLIFAPIENTRY RemoveState(OGLIF_HANDLE CodeTree, OGLIF_HANDLE State)
{
    OGLIF_ERROR result = OGLIF_INVALIDPARAMETER;
    if(CodeTree != OGLIF_INVALIDHANDLE && State != OGLIF_INVALIDHANDLE)
    {
        Page* page = GetPage(CodeTree);
        if(page != 0)
        {
            auto* tree = reinterpret_cast<OGLIF::IntermediateTree*>(&page->Mapping.TreeOffset);
            if(tree->ContainsState(reinterpret_cast<OGLIF::StateNode*>(State)))
            {
                OGLIF::StateNode* state = reinterpret_cast<OGLIF::StateNode*>(State);
                if(tree->RemoveState(state->Id))
                {
                    result = OGLIF_OK;
                }
            }
        }
    }
    return result;
}

OGLIF_ERROR OGLIFAPIENTRY RemoveVariable(OGLIF_HANDLE CodeTree, OGLIF_HANDLE Variable)
{
    OGLIF_ERROR result = OGLIF_INVALIDPARAMETER;
    if(CodeTree != OGLIF_INVALIDHANDLE && Variable != OGLIF_INVALIDHANDLE)
    {
        Page* page = GetPage(CodeTree);
        if(page != 0)
        {
            auto* tree = reinterpret_cast<OGLIF::IntermediateTree*>(&page->Mapping.TreeOffset);
            if(tree->ContainsVariable(reinterpret_cast<OGLIF::VariableNode*>(Variable)))
            {
                OGLIF::VariableNode* variable = reinterpret_cast<OGLIF::VariableNode*>(Variable);
                if(tree->RemoveVariable(variable->Id))
                {
                    result = OGLIF_OK;
                }
            }
        }
    }
    return result;
}
