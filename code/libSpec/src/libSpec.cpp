#include "tinyxml2.h"
#include "libSpec.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace tinyxml2;

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

void ObtainConstants(XMLElement* registry, unsigned int* &constantsCount, Constant** &constants);
void ObtainCommands(XMLElement* registry, unsigned int* &functionCount, Function** &functions);
void ObtainFeatures(XMLElement* registry, unsigned int* functionCount, Function** functions);

int SPECAPIENTRY readSpecs(const char* xmlData, unsigned int bytes,
    Function** functions, unsigned int* functionCount,
    Constant** constants, unsigned int* constantsCount)
{
    XMLDocument document;
    if (document.Parse(xmlData, bytes) == XMLError::XML_NO_ERROR)
    {
        XMLElement* registry = document.FirstChildElement("registry");
        if (registry)
        {
            ObtainConstants(registry, constantsCount, constants);
            ObtainCommands(registry, functionCount, functions);
            ObtainFeatures(registry, functionCount, functions);
        }
    }
    return 0;
}

void SPECAPIENTRY freeSpecs(Function** functions, const unsigned int functionCount, 
    Constant** constants, const unsigned int constantCount)
{
    unsigned int i = 0, j = 0;
    for (; i < functionCount; ++i)
    {
        delete[] (*functions)[i].Name;
        delete[] (*functions)[i].Result;
        for (j = 0; j < (*functions)[i].ParameterCount; ++j)
        {
            delete[] (*functions)[i].Parameters[j].Name;
            delete[] (*functions)[i].Parameters[j].Type;
            delete[] (*functions)[i].Parameters[j].PostType;
        }
        delete[] (*functions)[i].Parameters;

        for (j = 0; j < (*functions)[i].FeatureCount; ++j)
        {
            delete[](*functions)[i].Features[j].API;
            delete[](*functions)[i].Features[j].Number;
        }
        delete[](*functions)[i].Features;
    }
    delete[] * functions;
    *functions = NULL;

    for(i = 0; i < constantCount; ++i)
    {
        delete[](*constants)[i].Name;
        delete[](*constants)[i].Value;
        delete[](*constants)[i].API;
    }
    delete[] * constants;
    *constants = NULL;
}

void ObtainConstants(XMLElement* registry, unsigned int* &constantsCount, Constant** &constants)
{
    unsigned int constantEntries = 0;
    XMLElement* enumsEntry = registry->FirstChildElement("enums");
    while(enumsEntry)
    {
        XMLElement* enumEntry = enumsEntry->FirstChildElement("enum");
        while(enumEntry)
        {
            const char* value = enumEntry->Attribute("value");
            const char* name = enumEntry->Attribute("name");
            const char* api = enumEntry->Attribute("api");
            if(value && name)
            {
                ++constantEntries;
            }
            enumEntry = enumEntry->NextSiblingElement();
        }
        enumsEntry = enumsEntry->NextSiblingElement();
    }

    *constantsCount = constantEntries;
    *constants = new Constant[constantEntries];
    unsigned int i = 0;
    enumsEntry = registry->FirstChildElement("enums");
    while(enumsEntry)
    {
        enumsEntry->Attribute("namespace");
        XMLElement* enumEntry = enumsEntry->FirstChildElement("enum");
        while(enumEntry)
        {
            const char* value = enumEntry->Attribute("value");
            const char* name = enumEntry->Attribute("name");
            const char* api = enumEntry->Attribute("api");
            const char* type = enumEntry->Attribute("type");
            if(value && name)
            {
                int len = strlen(value);
                (*constants)[i].Value = new char[len + 1];
                strcpy((*constants)[i].Value, value);

                len = strlen(name);
                (*constants)[i].Name = new char[len + 1];
                strcpy((*constants)[i].Name, name);

                if(api)
                {
                    len = strlen(api);
                    (*constants)[i].API = new char[len + 1];
                    strcpy((*constants)[i].API, api);
                }
                else
                {
                    (*constants)[i].API = 0;
                }

                if(type)
                {
                    len = strlen(type);
                    (*constants)[i].Type = new char[len + 1];
                    strcpy((*constants)[i].Type, type);
                }
                else
                {
                    (*constants)[i].Type = 0;
                }

                ++i;
            }
            enumEntry = enumEntry->NextSiblingElement();
        }
        enumsEntry = enumsEntry->NextSiblingElement();
    }
}

void ObtainCommands(XMLElement* registry, unsigned int* &functionCount, Function** &functions)
{
    XMLElement* commands = registry->FirstChildElement("commands");
    if(commands)
    {
        // reserve memory
        unsigned int commandAmount = 0;
        XMLElement* cmd = commands->FirstChildElement("command");
        while(cmd)
        {
            ++commandAmount;
            // if (cmd->FirstChildElement("vecequiv"))
            //   ++commandAmount;
            //if (cmd->FirstChildElement("alias"))
            //  ++commandAmount;
            cmd = cmd->NextSiblingElement();
        }

        // fill data
        *functionCount = commandAmount;
        *functions = new Function[commandAmount];
        cmd = commands->FirstChildElement("command");
        for(unsigned int i = 0; i < commandAmount; ++i)
        {
            (*functions)[i].Name = 0;
            (*functions)[i].Result = 0;
            XMLElement* proto = cmd->FirstChildElement("proto");
            if(proto)
            {
                XMLElement* name = proto->FirstChildElement("name");
                if(name)
                {
                    int len = strlen(name->FirstChild()->Value());
                    (*functions)[i].Name = new char[len + 1];
                    strcpy((*functions)[i].Name, name->FirstChild()->Value());
                }
                XMLElement* resultType = proto->FirstChildElement("ptype");
                if(resultType)
                {
                    int len = strlen(resultType->FirstChild()->Value());
                    (*functions)[i].Result = new char[len + 1];
                    strcpy((*functions)[i].Result, resultType->FirstChild()->Value());
                }
                else
                {
                    int len = strlen("void");
                    (*functions)[i].Result = new char[len +1];
                    strcpy((*functions)[i].Result, "void");
                }
            }

            unsigned int parameterAmount = 0;
            XMLElement* param = cmd->FirstChildElement("param");
            while(param && strcmp(param->Value(), "param") == 0)
            {
                ++parameterAmount;
                param = param->NextSiblingElement();
            }

            (*functions)[i].ParameterCount = parameterAmount;

            (*functions)[i].Parameters = 0;
            if(parameterAmount)
            {
                (*functions)[i].Parameters = new Parameter[parameterAmount];
                param = cmd->FirstChildElement("param");
                for(unsigned int j = 0; j < parameterAmount; ++j)
                {
                    (*functions)[i].Parameters[j].Name = 0;
                    XMLElement* name = param->FirstChildElement("name");
                    if(name)
                    {
                        int len = strlen(name->FirstChild()->Value());
                        if(strcmp(name->FirstChild()->Value(), "residences") == 0)
                            len = len;
                        (*functions)[i].Parameters[j].Name = new char[len + 1];
                        strcpy((*functions)[i].Parameters[j].Name, name->FirstChild()->Value());
                    }

                    (*functions)[i].Parameters[j].Type = 0;
                    (*functions)[i].Parameters[j].PostType = 0;
                    (*functions)[i].Parameters[j].ByteSize = 0;
                    (*functions)[i].Parameters[j].IsConst = false;
                    (*functions)[i].Parameters[j].IsPointer = false;
                    XMLElement* type = param->FirstChildElement("ptype");
                    if(type)
                    {
                        int len = strlen(type->FirstChild()->Value());
                        (*functions)[i].Parameters[j].Type = new char[len + 1];

                        strcpy((*functions)[i].Parameters[j].Type, type->FirstChild()->Value());

                        if(type->PreviousSibling() && strstr(type->PreviousSibling()->Value(), "const") != 0)
                            (*functions)[i].Parameters[j].IsConst = true;

                        if(type->NextSibling() && strcmp(type->NextSibling()->Value(), "name") != 0)
                        {
                            int len = strlen(type->NextSibling()->Value());
                            (*functions)[i].Parameters[j].PostType = new char[len + 1];
                            strcpy((*functions)[i].Parameters[j].PostType, type->NextSibling()->Value());
                            if(strstr(type->NextSibling()->Value(), "*") != 0)
                                (*functions)[i].Parameters[j].IsPointer = true;
                        }
                        (*functions)[i].Parameters[j].ByteSize = stringTypeToSize((*functions)[i].Parameters[j].Type, (*functions)[i].Parameters[j].IsPointer);
                    }
                    else
                        if(param->FirstChild() && strcmp(param->FirstChild()->Value(), "name") != 0)
                        {
                            (*functions)[i].Parameters[j].Type = new char[5];
                            strcpy((*functions)[i].Parameters[j].Type, "void");
                            if(strstr(param->FirstChild()->Value(), "const") != 0)
                                (*functions)[i].Parameters[j].IsConst = true;
                            if(strstr(param->FirstChild()->Value(), "*") != 0)
                                (*functions)[i].Parameters[j].IsPointer = true;

                            const char* p = strstr(param->FirstChild()->Value(), "void");
                            if(p)
                            {
                                int len = strlen(p + 4);
                                (*functions)[i].Parameters[j].PostType = new char[len + 1];
                                strcpy((*functions)[i].Parameters[j].PostType, p + 4);
                            }
                        }

                    do
                    {
                        param = param->NextSiblingElement();
                    } while(param && strcmp(param->Value(), "param") != 0);
                }
            }

            (*functions)[i].FeatureCount = 0;
            (*functions)[i].Features = 0;

            cmd = cmd->NextSiblingElement();
        }
    }
}

void ObtainFeatures(XMLElement* registry, unsigned int* functionCount, Function** functions)
{
    XMLElement* feature = registry->FirstChildElement("feature");
    while(feature)
    {
        XMLElement* requireOrRemove = feature->FirstChildElement();
        while(requireOrRemove)
        {
            bool isRequire = strcmp(requireOrRemove->Name(), "require") == 0;
            bool isRemove = strcmp(requireOrRemove->Name(), "remove") == 0;
            if(isRemove || isRequire)
            {
                XMLElement* commandOrEnum = requireOrRemove->FirstChildElement();
                while(commandOrEnum)
                {
                    bool isEnum = strcmp(commandOrEnum->Name(), "enum") == 0;
                    bool isCommand = strcmp(commandOrEnum->Name(), "command") == 0;
                    if(isEnum || isCommand)
                    {
                        const char* name = commandOrEnum->Attribute("name");

                        if(isCommand)
                        {
                            for(unsigned int i = 0; i < *functionCount; ++i)
                            {
                                if(strcmp((*functions)[i].Name, name) == 0)
                                {
                                    ++(*functions)[i].FeatureCount;
                                    break;
                                }
                            }
                        }
                    }
                    commandOrEnum = commandOrEnum->NextSiblingElement();
                }
            }
            requireOrRemove = requireOrRemove->NextSiblingElement();
        }

        do
        {
            feature = feature->NextSiblingElement();
        } while(feature && strcmp(feature->Value(), "feature") != 0);
    }

    // reserve memory
    for(unsigned int i = 0; i < *functionCount; ++i)
    {
        (*functions)[i].Features = new Feature[(*functions)[i].FeatureCount];
        for(unsigned int j = 0; j < (*functions)[i].FeatureCount; ++j)
        {
            (*functions)[i].Features[j].API = 0;
            (*functions)[i].Features[j].Number = 0;
            (*functions)[i].Features[j].introduced = false;
            (*functions)[i].Features[j].removed = false;
        }
    }

    // update feature data
    feature = registry->FirstChildElement("feature");
    while(feature)
    {
        const char* api = feature->Attribute("api");
        const char* number = feature->Attribute("number");
        unsigned int apiLen = strlen(api) + 1;
        unsigned int numberLen = strlen(number) + 1;

        XMLElement* requireOrRemove = feature->FirstChildElement();
        while(requireOrRemove)
        {
            bool isRequire = strcmp(requireOrRemove->Name(), "require") == 0;
            bool isRemove = strcmp(requireOrRemove->Name(), "remove") == 0;
            if(isRemove || isRequire)
            {
                XMLElement* commandOrEnum = requireOrRemove->FirstChildElement();
                while(commandOrEnum)
                {
                    bool isEnum = strcmp(commandOrEnum->Name(), "enum") == 0;
                    bool isCommand = strcmp(commandOrEnum->Name(), "command") == 0;
                    if(isEnum || isCommand)
                    {
                        const char* name = commandOrEnum->Attribute("name");

                        if(isCommand)
                        {
                            for(unsigned int i = 0; i < *functionCount; ++i)
                            {
                                if(strcmp((*functions)[i].Name, name) == 0)
                                {
                                    for(unsigned int j = 0; j < (*functions)[i].FeatureCount; ++j)
                                    {
                                        if((*functions)[i].Features[j].API == 0)
                                        {
                                            (*functions)[i].Features[j].API = new char[apiLen];
                                            (*functions)[i].Features[j].Number = new char[numberLen];
                                            strcpy((*functions)[i].Features[j].API, api);
                                            strcpy((*functions)[i].Features[j].Number, number);
                                            (*functions)[i].Features[j].introduced = isRequire;
                                            (*functions)[i].Features[j].removed = isRemove;
                                            break;
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    commandOrEnum = commandOrEnum->NextSiblingElement();
                }
            }
            requireOrRemove = requireOrRemove->NextSiblingElement();
        }

        do
        {
            feature = feature->NextSiblingElement();
        } while(feature && strcmp(feature->Value(), "feature") != 0);
    }
}
