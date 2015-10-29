#include <RadonFramework/precompiled.hpp>
#include <RadonFramework/RadonInline.hpp>
#include <libOGLIF.h>

void* outOfMemoryAlloc(OGLIF_SIZE Bytes)
{
    return 0;
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
};  

CApiTest CApiTestImplementation;