#ifndef LIBSPEC_H
#define LIBSPEC_H

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

#ifndef SPECAPIENTRY
    #define SPECAPIENTRY APIENTRY
#endif

#ifdef SPEC_STATIC
    #define SPECAPI extern
#else
    #ifdef SPEC_BUILD
        #define SPECAPI extern __declspec(dllexport)
    #else
        #define SPECAPI extern __declspec(dllimport)
    #endif
#endif
#else
    #ifdef SPEC_STATIC
        #define SPECAPI extern
    #else
        #if defined(__GNUC__) && __GNUC__>=4
            #define SPECAPI extern __attribute__ ((visibility("default")))
        #elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
            #define SPECAPI extern __global
        #else
            #define SPECAPI extern
        #endif
    #endif

    #ifndef SPECAPIENTRY
        #define SPECAPIENTRY
    #endif
#endif

#ifdef __cplusplus
    extern "C" {
#endif

typedef struct
{
    char* Name;
    char* Type;
    char* PostType;
    unsigned int ByteSize;
    bool IsConst;
    bool IsPointer;
}Parameter;

typedef struct
{
    char* API;
    char* Number;
    bool introduced;
    bool removed;
}Feature;

typedef struct 
{
    char* Name;
    char* Supported;
}Extension;

typedef struct
{
    unsigned int FeatureCount;
    unsigned int ParameterCount;
    unsigned int ExtensionCount;
    char* Name;
    char* Result;
    Feature* Features;
    Parameter* Parameters;
    Extension* Extensions;
}Function;

typedef struct  
{
    char* Value;
    char* Name;
    char* API;
    char* Type;
}Constant;

SPECAPI int SPECAPIENTRY readSpecs(const char* xmlData, unsigned int bytes, 
    Function** functions, unsigned int* functionCount, Constant** constants, 
    unsigned int* constantsCount);

SPECAPI void SPECAPIENTRY freeSpecs(Function** functions, 
    const unsigned int functionCount, Constant** constants, 
    const unsigned int constantCount);

#ifdef __cplusplus
    }
#endif

#endif // LIBSPEC_H