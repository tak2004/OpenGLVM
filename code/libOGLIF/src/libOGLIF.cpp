#define OGLIF_BUILD
#include "libOGLIF.h"
#include "SmallObjectAllocator.hpp"
#include <string.h>
#include <new>

#if defined(_MSC_VER) && _MSC_VER < 1400
typedef __int64 GLint64EXT;
typedef unsigned __int64 GLuint64EXT;
#elif defined(_MSC_VER) || defined(__BORLANDC__)
typedef signed long long GLint64EXT;
typedef unsigned long long GLuint64EXT;
#else
#  if defined(__MINGW32__) || defined(__CYGWIN__)
#include <inttypes.h>
#  endif
typedef int64_t GLint64EXT;
typedef uint64_t GLuint64EXT;
#endif

typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLboolean;
typedef signed char GLbyte;
typedef short GLshort;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned long GLulong;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;
typedef GLint64EXT  GLint64;
typedef GLuint64EXT GLuint64;
typedef struct __GLsync *GLsync;
typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptrARB;
typedef ptrdiff_t GLsizeiptrARB;
typedef char GLcharARB;
typedef unsigned int GLhandleARB;
typedef unsigned short GLhalfARB;
typedef unsigned short GLhalfNV;
typedef GLintptr GLvdpauSurfaceNV;

namespace OGLIF {

int stringTypeToSize(const char* type, bool isPointer)
{
    if (isPointer) return sizeof(void*);
    if (strcmp(type, "GLenum") == 0)               return sizeof(GLenum);
    if (strcmp(type, "GLbitfield") == 0)           return sizeof(GLbitfield);
    if (strcmp(type, "GLuint") == 0)               return sizeof(GLuint);
    if (strcmp(type, "GLint") == 0)                return sizeof(GLint);
    if (strcmp(type, "GLsizei") == 0)              return sizeof(GLsizei);
    if (strcmp(type, "GLboolean") == 0)            return sizeof(GLboolean);
    if (strcmp(type, "GLbyte") == 0)               return sizeof(GLbyte);
    if (strcmp(type, "GLshort") == 0)              return sizeof(GLshort);
    if (strcmp(type, "GLubyte") == 0)              return sizeof(GLubyte);
    if (strcmp(type, "GLushort") == 0)             return sizeof(GLushort);
    if (strcmp(type, "GLulong") == 0)              return sizeof(GLulong);
    if (strcmp(type, "GLfloat") == 0)              return sizeof(GLfloat);
    if (strcmp(type, "GLclampf") == 0)             return sizeof(GLclampf);
    if (strcmp(type, "GLdouble") == 0)             return sizeof(GLdouble);
    if (strcmp(type, "GLclampd") == 0)             return sizeof(GLclampd);
    if (strcmp(type, "GLvoid") == 0)               return 0;
    if (strcmp(type, "GLint64") == 0)              return sizeof(GLint64);
    if (strcmp(type, "GLuint64") == 0)             return sizeof(GLuint64);
    if (strcmp(type, "GLsync") == 0)               return sizeof(GLsync);
    if (strcmp(type, "GLchar") == 0)               return sizeof(GLchar);
    if (strcmp(type, "GLintptr") == 0)             return sizeof(GLintptr);
    if (strcmp(type, "GLsizeiptr") == 0)           return sizeof(GLsizeiptr);
    if (strcmp(type, "GLintptrARB") == 0)          return sizeof(GLintptrARB);
    if (strcmp(type, "GLsizeiptrARB") == 0)        return sizeof(GLsizeiptrARB);
    if (strcmp(type, "GLcharARB") == 0)            return sizeof(GLcharARB);
    if (strcmp(type, "GLhandleARB") == 0)          return sizeof(GLhandleARB);
    if (strcmp(type, "GLhalfARB") == 0)            return sizeof(GLhalfARB);
    if (strcmp(type, "GLhalfNV") == 0)             return sizeof(GLhalfNV);
    if (strcmp(type, "GLint64EXT") == 0)           return sizeof(GLint64EXT);
    if (strcmp(type, "GLuint64EXT") == 0)          return sizeof(GLuint64EXT);
    if (strcmp(type, "GLvdpauSurfaceNV") == 0)     return sizeof(GLvdpauSurfaceNV);
    if (strcmp(type, "struct _cl_context *") == 0) return sizeof(void*);
    if (strcmp(type, "struct _cl_event *") == 0)   return sizeof(void*);
    if (strcmp(type, "GLDEBUGPROCARB") == 0)       return sizeof(void*);
    if (strcmp(type, "GLDEBUGPROC") == 0)          return sizeof(void*);
    if (strcmp(type, "GLDEBUGPROCAMD") == 0)       return sizeof(void*);
    if (strcmp(type, "GLfixed") == 0)              return sizeof(GLint);
    return -1;
}

OGLIF_U32 CalculateFNV(const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes)
{
    OGLIF_U32 hash = 2166136261u;
    for(OGLIF_SIZE i = 0; i < NameSizeInBytes; ++i)
    {
        hash ^= *Name++;
        hash *= 16777619u;
    }
    return hash;
}

class StateNode:public SmallObject
{
public:
    StateNode(const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes)
    :m_Name(0)
    ,m_NameSizeInBytes(0)
    ,Next(0)
    {
        m_Hash = CalculateFNV(Name, NameSizeInBytes);
    }

    virtual ~StateNode()
    {

    }

    StateNode* Next;
private:
    OGLIF_U8* m_Name;
    OGLIF_U32 m_NameSizeInBytes;
    OGLIF_U32 m_Hash;
};

class VariableNode:public SmallObject
{
public:
    VariableNode(const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes, OGLIF_TYPE Type, 
                 const OGLIF_U8* Value, OGLIF_U32 ValueSizeInBytes)
    :m_Name(0)
    ,m_NameSizeInBytes(0)
    ,Next(0)
    ,m_Type(Type)
    {
        m_Hash = CalculateFNV(Name, NameSizeInBytes);
    }

    virtual ~VariableNode()
    {

    }

    VariableNode* Next;
private:
    OGLIF_U8* m_Name;
    OGLIF_U32 m_NameSizeInBytes;
    OGLIF_U32 m_Hash;
    OGLIF_TYPE m_Type;
};

class IntermediateTree
{
public:
    IntermediateTree(void* Scratchpad, OGLIF_SIZE Size,
        void* (*Allocate)(OGLIF_SIZE Bytes), 
        void(*Deallocate)(void* MemoryPointer), 
        void* (*Reallocate)(void* MemoryPointer, OGLIF_SIZE NewSize))
    :m_Scratchpad(Scratchpad)
    ,m_Size(Size)
    ,m_InUse(sizeof(IntermediateTree))
    ,m_Allocate(Allocate)
    ,m_Deallocate(Deallocate)
    ,m_Reallocate(Reallocate)
    ,m_FirstState(0)
    ,m_LastState(0)
    {
        m_SmallObjectPool = new(reinterpret_cast<OGLIF_U8*>(m_Scratchpad)+m_InUse) SmallObjectAllocator(128,4096,Allocate,Deallocate,Reallocate);
        m_InUse += sizeof(SmallObjectAllocator);
    }

    bool AddState(const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes)
    {
        bool result = false;
        SmallObject* node = m_SmallObjectPool->Allocate();
        if(node)
        {
            StateNode* state = new(node)StateNode(Name, NameSizeInBytes);
            if(m_FirstState == 0)
            {
                m_FirstState = state;
                m_LastState = state;
            }
            else
            {
                m_LastState->Next = state;
                m_LastState = state;
            }

            result = true;
        }
        return result;
    }

    bool AddVariable(const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes,
        OGLIF_TYPE Type, const OGLIF_U8* Value, OGLIF_U32 ValueSizeInBytes)
    {
        bool result = false;
        SmallObject* node = m_SmallObjectPool->Allocate();
        if(node)
        {
            VariableNode* variable = new(node)VariableNode(Name, NameSizeInBytes, 
                Type, Value, ValueSizeInBytes);
            if(m_FirstVariable == 0)
            {
                m_FirstVariable = variable;
                m_LastVariable = variable;
            }
            else
            {
                m_LastVariable->Next = variable;
                m_LastVariable = variable;
            }

            result = true;
        }
        return result;
    }

    void* m_Scratchpad;
    void* (*m_Allocate)(OGLIF_SIZE);
    void  (*m_Deallocate)(void*);
    void* (*m_Reallocate)(void*, OGLIF_SIZE);
private:
    OGLIF_SIZE m_Size;
    OGLIF_SIZE m_InUse;
    SmallObjectAllocator* m_SmallObjectPool;
    StateNode* m_FirstState;
    StateNode* m_LastState;
    VariableNode* m_FirstVariable;
    VariableNode* m_LastVariable;
};

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
    void* (*Allocate)(OGLIF_SIZE Bytes), void(*Deallocate)(void* MemoryPointer),
    void* (*Reallocate)(void* MemoryPointer, OGLIF_SIZE NewSize))
{
    OGLIF_HANDLE result = OGLIF_INVALIDHANDLE;
    if(Data != 0 && DataSize >= sizeof(OGLIF_Identifier) && Allocate != 0 && Deallocate != 0 &&
       IsRightFormat(Data, DataSize))
    {
        IntermediateTree* tree = 0;

        auto* identifier = reinterpret_cast<const OGLIF_Identifier*>(Data);
        auto* header = reinterpret_cast<const OGLIF_Header*>(identifier+1);
        const OGLIF_Section* sections = 0;
        if(header->DataSize > 0)
        {
            sections = reinterpret_cast<const OGLIF_Section*>(header + 1);
        }

        void* p = Allocate(4096);
        if(p)
        {
            // placement new on allocated memory block
            tree = new(p)IntermediateTree(p, 4096, Allocate, Deallocate, Reallocate);
        }

        if(tree)
        {
            if(sections)
            {

            }
            result = reinterpret_cast<OGLIF_HANDLE>(tree);
        }
    }
    return result;
}

}

OGLIF_HANDLE OGLIFAPIENTRY CreateHandle(void* (*Allocate)(OGLIF_SIZE Bytes), 
    void(*Deallocate)(void* MemoryPointer),
    void* (*Reallocate)(void* MemoryPointer, OGLIF_SIZE NewSize))
{
    OGLIF_HANDLE result = OGLIF_INVALIDHANDLE;
    if(Allocate != 0 && Deallocate != 0)
    {
        OGLIF::IntermediateTree* tree = 0;
        void* p = Allocate(4096);
        if(p)
        {
            // placement new on allocated memory block
            tree = new(p)OGLIF::IntermediateTree(p, 4096, Allocate, Deallocate, Reallocate);
        }

        if(tree)
        {
            result = reinterpret_cast<OGLIF_HANDLE>(tree);
        }
    }
    return result;
}

void OGLIFAPIENTRY FreeHandle(OGLIF_HANDLE Handle)
{
    if(Handle != OGLIF_INVALIDHANDLE)
    {
        OGLIF::IntermediateTree* tree = reinterpret_cast<OGLIF::IntermediateTree*>(Handle);
        void* p = tree->m_Scratchpad;
        void (*deallocate)(void*) = tree->m_Deallocate;

        tree->~IntermediateTree();
        deallocate(p);
    }
}

void OGLIFAPIENTRY AddState(OGLIF_HANDLE Handle, const OGLIF_U8* Name,
                            OGLIF_U32 NameSizeInBytes)
{
    if(Handle != OGLIF_INVALIDHANDLE)
    {
        OGLIF::IntermediateTree* tree = reinterpret_cast<OGLIF::IntermediateTree*>(Handle);
        tree->AddState(Name, NameSizeInBytes);
    }
}

bool IsValidValue(OGLIF_TYPE Type, const OGLIF_U8* Value, OGLIF_U32 ValueSizeInBytes)
{
    return false;
}

void OGLIFAPIENTRY AddVariable(OGLIF_HANDLE Handle, const OGLIF_U8* Name, 
    OGLIF_U32 NameSizeInBytes, OGLIF_TYPE Type, const OGLIF_U8* Value, 
    OGLIF_U32 ValueSizeInBytes)
{
    if(Handle != OGLIF_INVALIDHANDLE && IsValidValue(Type, Value, ValueSizeInBytes))
    {
        OGLIF::IntermediateTree* tree = reinterpret_cast<OGLIF::IntermediateTree*>(Handle);
        tree->AddVariable(Name, NameSizeInBytes, Type, Value, ValueSizeInBytes);
    }
}
