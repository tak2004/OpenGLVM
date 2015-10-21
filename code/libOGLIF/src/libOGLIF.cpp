#define OGLIF_BUILD
#include "libOGLIF.h"

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

class IntermediateTree
{
public:
    IntermediateTree(void* Location, void* (*Allocate)(OGLIF_SIZE Bytes), 
        void(*Deallocate)(void* MemoryPointer), 
        void* (*Reallocate)(void* MemoryPointer, OGLIF_SIZE NewSize))
    :m_Location(Location)
    ,m_Allocate(Allocate)
    ,m_Deallocate(Deallocate)
    ,m_Reallocate(Reallocate)
    {

    }

    void* m_Location;
    void* (*m_Allocate)(OGLIF_SIZE);
    void  (*m_Deallocate)(void*);
    void* (*m_Reallocate)(void*, OGLIF_SIZE);
};

OGLIF_HANDLE OGLIFAPIENTRY ParseFromMemory(const OGLIF_U8* Data, OGLIF_U32 DataSize,
    void* (*Allocate)(OGLIF_SIZE Bytes), void(*Deallocate)(void* MemoryPointer),
    void* (*Reallocate)(void* MemoryPointer, OGLIF_SIZE NewSize))
{
    OGLIF_HANDLE result = 0;
    if(Data != 0 && DataSize > 0 && Allocate != 0 && Deallocate != 0)
    {
        IntermediateTree* tree = 0;
        void* p = Allocate(DataSize * 2);
        if(p)
        {
            // copy data

            p = reinterpret_cast<OGLIF_U8*>(p)+DataSize;
            // placement new on allocated memory block
            tree = new(p) IntermediateTree(p, Allocate, Deallocate, Reallocate);            
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
        IntermediateTree* tree = reinterpret_cast<IntermediateTree*>(Handle);
        void* p = tree->m_Location;
        void (*deallocate)(void*) = tree->m_Deallocate;

        tree->~IntermediateTree();
        deallocate(p);
    }
}

