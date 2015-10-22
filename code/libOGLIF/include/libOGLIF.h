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

#ifndef OGLIF_INVALIDHANDLE
#define OGLIF_INVALIDHANDLE 0
#endif

#ifndef OGLIF_FOURCC
#define OGLIF_FOURCC 'GLIF'
#endif

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
    void* (*Allocate)(OGLIF_SIZE Bytes)=malloc, void(*Deallocate)(void* MemoryPointer)=free,
    void* (*Reallocate)(void* MemoryPointer, OGLIF_SIZE NewSize)=realloc);

OGLIFAPI OGLIF_HANDLE OGLIFAPIENTRY CreateHandle(void* (*Allocate)(OGLIF_SIZE Bytes) = malloc, 
    void(*Deallocate)(void* MemoryPointer) = free,
    void* (*Reallocate)(void* MemoryPointer, OGLIF_SIZE NewSize) = realloc);

OGLIFAPI void OGLIFAPIENTRY FreeHandle(OGLIF_HANDLE Handle);

/**
 * @param Name Takes a \0 terminated Utf8 string.
 */
OGLIFAPI void OGLIFAPIENTRY AddState(OGLIF_HANDLE Handle, const OGLIF_U8* Name, 
    OGLIF_U32 NameSizeInBytes);

/**
* @param Name Takes a \0 terminated Utf8 string.
*/
OGLIFAPI void OGLIFAPIENTRY AddVariable(OGLIF_HANDLE Handle, const OGLIF_U8* Name,
                                        OGLIF_U32 NameSizeInBytes, OGLIF_TYPE Type,
                                        const OGLIF_U8* Value, OGLIF_U32 ValueSizeInBytes);

// OGLIFAPI void OGLIFAPIENTRY AddCommand(OGLIF_HANDLE Handle, OGLIF_U16 CommandId,
//                                        const OGLIF_U8* ParameterData, 
//                                        OGLIF_U32 DataSizeInBytes);

#ifdef __cplusplus
    }
#endif

#endif // LIBOGLIF_H