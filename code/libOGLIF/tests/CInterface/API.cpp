#include <RadonFramework/precompiled.hpp>
#include <RadonFramework/RadonInline.hpp>
#include <libOGLIF.h>

void* outOfMemoryAlloc(OGLIF_SIZE Bytes)
{
    return 0;
}

OGLIF_SIZE used = 0;

// CodeTree will take a 4096 block but all other allocation will fail
void* codeTreeOnlyAlloc(OGLIF_SIZE Bytes)
{
    void* result = 0;
    if(used > 4096)
    { 
        result = malloc(Bytes);
        used += Bytes;
    }
    return result;
}

void ResetCodeTreeOnlyAlloc()
{
    used=0;
}

class CApiTest :public RF_Test::TestSuite
{
public:
    CApiTest()
    :TestSuite("OGLIF::CApiTest")
    {
        AddTest(MakeDelegate(this, &CApiTest::Constructor),
            "CApiTest::Constructor", "Constructor");
        AddTest(MakeDelegate(this, &CApiTest::Destructor),
            "CApiTest::Destructor", "Destructor");
        AddTest(MakeDelegate(this, &CApiTest::AddStates),
                "CApiTest::AddStates", "Add states");
        AddTest(MakeDelegate(this, &CApiTest::RemoveStates),
                "CApiTest::RemoveStates", "Remove states");
        AddTest(MakeDelegate(this, &CApiTest::AddVariables),
                "CApiTest::AddVariables, "Add variables");
        AddTest(MakeDelegate(this, &CApiTest::RemoveVariables),
                "CApiTest::RemoveVariables", "Remove variables");
    }
    
    RF_Type::Bool Constructor()
    {
        RF_Type::Bool result = false;
        OGLIF_HANDLE handle1, handle2, handle3, nohandle;
        result = CreateCodeTree(malloc, free, realloc, &handle1) == OGLIF_OK &&
                 CreateCodeTree(malloc, free, 0, &handle2) == OGLIF_OK &&
                 CreateCodeTree(0, 0, 0, &handle3) == OGLIF_OK &&
                 CreateCodeTree(malloc, 0, 0, &nohandle) == OGLIF_INVALIDPARAMETER &&
                 CreateCodeTree(0, free, 0, &nohandle) == OGLIF_INVALIDPARAMETER &&
                 CreateCodeTree(0, 0, realloc, &nohandle) == OGLIF_INVALIDPARAMETER &&
                 CreateCodeTree(0, 0, 0, 0) == OGLIF_INVALIDPARAMETER &&
                 CreateCodeTree(outOfMemoryAlloc, free, 0, &nohandle) == OGLIF_OUTOFMEMORY &&
                 handle1 != OGLIF_INVALIDHANDLE &&
                 handle2 != OGLIF_INVALIDHANDLE &&
                 handle3 != OGLIF_INVALIDHANDLE;
        FreeCodeTree(handle1);
        FreeCodeTree(handle2);
        FreeCodeTree(handle3);
        return result;
    }

    RF_Type::Bool Destructor()
    {
        RF_Type::Bool result = false;
        OGLIF_HANDLE handle1 = OGLIF_INVALIDHANDLE;
        OGLIF_HANDLE handle2 = OGLIF_INVALIDHANDLE;
        OGLIF_HANDLE handle3 = OGLIF_INVALIDHANDLE;
        CreateCodeTree(malloc, free, realloc, &handle1);
        CreateCodeTree(malloc, free, realloc, &handle3);
        
        result = FreeCodeTree(handle3) == OGLIF_OK &&
                 FreeCodeTree(handle1) == OGLIF_OK &&
                 FreeCodeTree(handle2) == OGLIF_OK &&
                 FreeCodeTree(handle1) == OGLIF_INVALIDPARAMETER;

        return result;
    }

    RF_Type::Bool AddStates()
    {
        RF_Type::Bool result = false;
        const char* name = "test";
        OGLIF_HANDLE handle1 = OGLIF_INVALIDHANDLE;
        OGLIF_HANDLE handle2 = OGLIF_INVALIDHANDLE;
        OGLIF_HANDLE outOfMemoryCodeTree = OGLIF_INVALIDHANDLE;
        OGLIF_HANDLE codeTree;
        
        ResetCodeTreeOnlyAlloc();
        CreateCodeTree(0, 0, 0, &codeTree);
        CreateCodeTree(codeTreeOnlyAlloc, free, 0, &outOfMemoryCodeTree);

        result = AddState(codeTree, reinterpret_cast<const OGLIF_U8*>(name), sizeof(name), &handle1) == OGLIF_OK &&
                 AddState(codeTree, reinterpret_cast<const OGLIF_U8*>(name), sizeof(name), 0) == OGLIF_INVALIDPARAMETER &&
                 AddState(codeTree, 0, 0, 0) == OGLIF_INVALIDPARAMETER &&
                 AddState(codeTree, 0, 0, &handle2) == OGLIF_INVALIDPARAMETER &&
                 AddState(OGLIF_INVALIDHANDLE, reinterpret_cast<const OGLIF_U8*>(name), sizeof(name), &handle2) == OGLIF_INVALIDPARAMETER &&
                 AddState(OGLIF_INVALIDHANDLE, reinterpret_cast<const OGLIF_U8*>(name), sizeof(name), &handle2) == OGLIF_INVALIDPARAMETER;

        RemoveState(codeTree, handle1);
        FreeCodeTree(codeTree);

        return result;
    }

    RF_Type::Bool RemoveStates()
    {
        RF_Type::Bool result = false;
        OGLIF_HANDLE validCodeTree = OGLIF_INVALIDHANDLE;
        OGLIF_HANDLE invalidCodeTree = OGLIF_INVALIDHANDLE;
        OGLIF_HANDLE validState = OGLIF_INVALIDHANDLE;
        OGLIF_HANDLE invalidState = OGLIF_INVALIDHANDLE;
        const char* name = "test";
        
        CreateCodeTree(0, 0, 0, &validCodeTree);
        AddState(validCodeTree, reinterpret_cast<const OGLIF_U8*>(name), sizeof(name), &validState);

        result = RemoveState(invalidCodeTree, invalidState) == OGLIF_INVALIDPARAMETER &&
                 RemoveState(validCodeTree, invalidState) == OGLIF_INVALIDPARAMETER &&
                 RemoveState(validCodeTree, validState) == OGLIF_OK;

        RemoveState(validCodeTree, validState);
        FreeCodeTree(validCodeTree);

        return result;
    }


    RF_Type::Bool AddVariables()
    {
        RF_Type::Bool result = false;
        const char* name = "test";
        OGLIF_HANDLE handle1 = OGLIF_INVALIDHANDLE;
        OGLIF_HANDLE handle2 = OGLIF_INVALIDHANDLE;
        OGLIF_HANDLE outOfMemoryCodeTree = OGLIF_INVALIDHANDLE;
        OGLIF_HANDLE codeTree;

        ResetCodeTreeOnlyAlloc();

        result = CreateCodeTree(0, 0, 0, &codeTree) == OGLIF_OK &&
            CreateCodeTree(codeTreeOnlyAlloc, free, 0, &outOfMemoryCodeTree) == OGLIF_OK &&
            AddVariable(codeTree, reinterpret_cast<const OGLIF_U8*>(name), sizeof(name), 0,0,0, &handle1) == OGLIF_OK &&
            AddVariable(codeTree, reinterpret_cast<const OGLIF_U8*>(name), sizeof(name), 0) == OGLIF_INVALIDPARAMETER &&
            AddVariable(codeTree, 0, 0, 0) == OGLIF_INVALIDPARAMETER &&
            AddVariable(codeTree, 0, 0, &handle2) == OGLIF_INVALIDPARAMETER &&
            AddVariable(OGLIF_INVALIDHANDLE, reinterpret_cast<const OGLIF_U8*>(name), sizeof(name), &handle2) == OGLIF_INVALIDPARAMETER &&
            AddVariable(OGLIF_INVALIDHANDLE, reinterpret_cast<const OGLIF_U8*>(name), sizeof(name), &handle2) == OGLIF_INVALIDPARAMETER;
        return result;
    }

    RF_Type::Bool RemoveVariables()
    {
        RF_Type::Bool result = false;
        OGLIF_HANDLE validCodeTree = OGLIF_INVALIDHANDLE;
        OGLIF_HANDLE invalidCodeTree = OGLIF_INVALIDHANDLE;
        OGLIF_HANDLE validVariable = OGLIF_INVALIDHANDLE;
        OGLIF_HANDLE invalidVariable = OGLIF_INVALIDHANDLE;
        const char* name = "test";
        result = CreateCodeTree(0, 0, 0, &validCodeTree) == OGLIF_OK &&
            AddVariable(validCodeTree, reinterpret_cast<const OGLIF_U8*>(name), sizeof(name), &validVariable) == OGLIF_OK &&
            RemoveState(invalidCodeTree, invalidState) == OGLIF_INVALIDPARAMETER &&
            RemoveState(validCodeTree, invalidState) == OGLIF_INVALIDPARAMETER &&
            RemoveState(validCodeTree, validState) == OGLIF_OK &&
            FreeCodeTree(validCodeTree) == OGLIF_OK;
        return result;
    }
};  

CApiTest CApiTestImplementation;