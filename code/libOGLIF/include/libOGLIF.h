#ifndef LIBOGLIF_H
#define LIBOGLIF_H

#if defined(_WIN32)
    #ifndef APIENTRY
        #if defined(__MINGW32__) || defined(__CYGWIN__)
            #define APIENTRY __stdcall
        #elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__)
            #define APIENTRY __stdcall
        #else
            #define APIENTRY
        #endif
    #endif

    #ifndef OGLIFAPIENTRY
        #define OGLIFAPIENTRY APIENTRY
    #endif

    #ifdef OGLIF_STATIC
        #define OGLIFAPI extern
    #else
        #ifdef OGLIF_BUILD
            #define OGLIFAPI extern __declspec(dllexport)
        #else
            #define OGLIFAPI extern __declspec(dllimport)
        #endif
    #endif
#else
    #ifdef OGLIF_STATIC
        #define OGLIFAPI extern
    #else
        #if defined(__GNUC__) && __GNUC__>=4
            #define OGLIFAPI extern __attribute__ ((visibility("default")))
        #elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
            #define OGLIFAPI extern __global
        #else
            #define OGLIFAPI extern
        #endif
    #endif

    #ifndef OGLIFAPIENTRY
        #define OGLIFAPIENTRY
    #endif
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

// Use own types by set the defines before.
#ifndef OGLIF_U8
#define OGLIF_U8 uint8_t
#endif
#ifndef OGLIF_U16
#define OGLIF_U16 uint16_t
#endif
#ifndef OGLIF_U32
#define OGLIF_U32 uint32_t
#endif
#ifndef OGLIF_U64
#define OGLIF_U64 uint64_t
#endif
#ifndef OGLIF_HANDLE
#define OGLIF_HANDLE intptr_t
#endif
#ifndef OGLIF_SIZE
#define OGLIF_SIZE size_t
#endif
#ifndef OGLIF_TYPE
#define OGLIF_TYPE uint8_t
#endif
#ifndef OGLIF_I32
#define OGLIF_I32 int32_t
#endif
#ifndef OGLIF_ERROR
#define OGLIF_ERROR int32_t
#endif

// OGLIF_ERROR begin
static const OGLIF_ERROR OGLIF_OK = 0;
static const OGLIF_ERROR OGLIF_INVALIDPARAMETER = 1;
static const OGLIF_ERROR OGLIF_OUTOFMEMORY = 2;
// OGLIF_ERROR end

static const OGLIF_HANDLE OGLIF_INVALIDHANDLE = 0;

static const OGLIF_U32 OGLIF_FOURCC = 'GLIF';

typedef struct
{
    OGLIF_HANDLE Variable;
    OGLIF_TYPE Type;
    const OGLIF_U8* Value;
    OGLIF_U32 ValueSize;
}OGLIF_PARAMETER;

typedef struct
{
    OGLIF_U32 FourCC;
    OGLIF_U16  Version;
    OGLIF_U16  VendorVersion;
}OGLIF_Identifier;

typedef struct
{
    OGLIF_U32 DataSize;
    OGLIF_U64 DataHash;
}OGLIF_Header;

typedef struct
{
    OGLIF_U32 Type;
    OGLIF_U32 SectionSize;
}OGLIF_Section;

typedef struct
{
    OGLIF_U32 Identifier;
    OGLIF_U8  DataSizeInByte;
    OGLIF_U8* Data;
}OGLIF_Variable;

typedef struct
{
    OGLIF_U32 Hash;
    OGLIF_U8  NameSizeInByte;
    OGLIF_U8* NameUTF8;
}OGLIF_SymbolEntry;

typedef struct
{
    OGLIF_Section Header;
    OGLIF_U32 StateId;
    OGLIF_U32 CodeSizeInByte;
    OGLIF_U8* Code;
}OGLIF_StateCodeSection;

typedef struct
{
    OGLIF_Section Header;
    OGLIF_U32 VariableCount;
    OGLIF_Variable* Variables;
}OGLIF_MemoryDataSection;

typedef struct
{
    OGLIF_Section Header;
    OGLIF_U32 SymbolCount;
    OGLIF_SymbolEntry* Symbols;
}OGLIF_SymbolLookupTableSection;

typedef void* (*Allocator)(OGLIF_SIZE Bytes);
typedef void  (*Deallocator)(void* MemoryPointer);
typedef void* (*Reallocator)(void* MemoryPointer, OGLIF_SIZE NewSize);

/** Brief Parse the specified memory block.
*
* Each OGLIF_HANDLE has it's own allocation function set to allow more efficient
* memory usage e.g. stack allocator for long living resources and frame allocator 
* for communication.
*
* @param Reallocate Set the parameter 0 to disable reallocation.
* @return Returns OGLIF_INVALIDHANDLE if an error occurred else a valid handle.
*/
OGLIFAPI OGLIF_HANDLE OGLIFAPIENTRY ParseFromMemory(const OGLIF_U8* Data, OGLIF_U32 DataSize,
    Allocator Allocate, Deallocator Deallocate, Reallocator Reallocate, OGLIF_HANDLE* Destination);

/** Brief Create a new code tree.
*
* Every code tree have a own set of memory functions to work with.
* This allows to optimize the work with an instance.
* 
* You can pass 0 for all three function pointer to use the default memory allocator.
*
* @param Reallocate Set the parameter 0 to disable reallocation.
*/
OGLIFAPI OGLIF_ERROR OGLIFAPIENTRY CreateCodeTree(Allocator Allocate,
    Deallocator Deallocate, Reallocator Reallocate, OGLIF_HANDLE* Destination);

OGLIFAPI OGLIF_ERROR OGLIFAPIENTRY FreeCodeTree(OGLIF_HANDLE CodeTree);

OGLIFAPI OGLIF_I32 OGLIFAPIENTRY BuildByteCode(OGLIF_HANDLE Handle, 
    OGLIF_U8* Data, OGLIF_U16 DataSize);

/**
 * @param Name Takes a \0 terminated Utf8 string.
 */
OGLIFAPI OGLIF_ERROR OGLIFAPIENTRY AddState(OGLIF_HANDLE CodeTree, 
    const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes, OGLIF_HANDLE* Destination);

OGLIFAPI OGLIF_ERROR OGLIFAPIENTRY RemoveState(OGLIF_HANDLE CodeTree,
    OGLIF_HANDLE State);

/**
* @param Name Takes a \0 terminated Utf8 string.
*/
OGLIFAPI OGLIF_ERROR OGLIFAPIENTRY AddVariable(OGLIF_HANDLE CodeTree,
    const OGLIF_U8* Name, OGLIF_U32 NameSizeInBytes, OGLIF_TYPE Type, 
    const OGLIF_U8* Value, OGLIF_U32 ValueSizeInBytes, OGLIF_HANDLE* Destination);

OGLIFAPI OGLIF_ERROR OGLIFAPIENTRY RemoveVariable(OGLIF_HANDLE CodeTree,
    OGLIF_HANDLE Variable);

OGLIFAPI OGLIF_HANDLE OGLIFAPIENTRY AddCommand(OGLIF_HANDLE CodeTree,
    OGLIF_HANDLE State, OGLIF_U16 CommandId, OGLIF_U8 ParameterCount,
    const OGLIF_PARAMETER* Parameters);

#ifdef __cplusplus
    }
#endif

#endif // LIBOGLIF_H