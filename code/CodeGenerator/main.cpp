#include <RadonFramework/precompiled.hpp>
#include <RadonFramework/Radon.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <libSpec.h>
#include <math.h>
#include <string.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b)) 

using namespace RadonFramework;
using namespace RadonFramework::IO;
using namespace RadonFramework::Memory;
using namespace RadonFramework::Diagnostics;
using namespace RadonFramework::Core::Types;

void writeTypeTraits(FILE* fd, unsigned int regcount, const char* fullType, 
                     const char* type, const char* cmd)
{
    unsigned int i;
    fprintf(fd, "template<>\n\
const GLOpCode::Type Get%sOpCode<%s>::COMMAND[] = {\n", cmd, fullType);

    for(i = 0; i < regcount - 1; ++i)
    {
        fprintf(fd, "\tGLOpCode::%s%sReg%d, \n", cmd, type, i);
    }
    fprintf(fd, "\tGLOpCode::%s%sReg%d\n};\n\n", cmd, type, i);

    fprintf(fd, "template<>\n\
struct Get%sOpCodeTrait<%s>\n\
{\n\
\tenum\n\
\t{\n\
\t\tSUPPORTED = true\n\
\t};\n\
};\n\n", cmd, fullType);
}

void writeConstants(FILE* fd, Constant* constants, unsigned int constantCount, 
    const char* guard)
{
    RF_Type::Size i;
    fprintf(fd,
"#ifndef %s\n\
#define %s\n\
\
namespace RadonFramework { namespace GL {\n\
\n", guard);

    for(i = 0; i < constantCount; ++i)
    {
        if(constants[i].API == 0 || strcmp(constants[i].API, "gl") == 0)
        {
            if(constants[i].Type == 0)
            {
                fprintf(fd, "const RF_Type::Int32 %s = %s;\n", constants[i].Name, constants[i].Value);
            }
            else
            {
                if(strcmp(constants[i].Type, "ull"))
                {
                    fprintf(fd, "const RF_Type::UInt64 %s = %s;\n", constants[i].Name, constants[i].Value);
                }
                else
                    if(strcmp(constants[i].Type, "u"))
                    {
                        fprintf(fd, "const RF_Type::UInt32 %s = %s;\n", constants[i].Name, constants[i].Value);
                    }
                    else
                    {
                        printf("Unhandled enum type '%s' of constant '%s'\n", constants[i].Type, constants[i].Name);
                    }
            }
        }
    }

    fprintf(fd,
"\n\
} }\n\
\n\
#ifndef RF_SHORTHAND_NAMESPACE_OGL\n\
#define RF_SHORTHAND_NAMESPACE_OGL\n\
    namespace RF_GL = RadonFramework::GL;\n\
#endif\n\
\n\
#endif // %s", guard);
}

void writeFunctionsHeader(FILE* fd, Function* functions, unsigned int functionCount,
    const char* guard)
{
    RF_Type::Size i,j;
    fprintf(fd,
"#ifndef %s\n\
#define %s\n\
n\
#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)\n\
#define WIN32_LEAN_AND_MEAN 1\n\
#include <windows.h>\n\
#endif\n\
\n\
#ifndef APIENTRY\n\
#define APIENTRY\n\
#endif\n\
#ifndef APIENTRYP\n\
#define APIENTRYP APIENTRY *\n\
#endif\n\
#ifndef GLAPI\n\
#define GLAPI extern\n\
#endif\n\
\n\
//namespace RadonFramework { namespace GL {\n\
\n", guard);

    for(i = 0; i < functionCount; ++i)
    {
        fprintf(fd, "typedef %s (*%sType)(", functions[i].Result, functions[i].Name);
        if(functions[i].ParameterCount > 0)
        {
            if(functions[i].ParameterCount > 1)
            {
                for(j = 0; j < functions[i].ParameterCount - 1; ++j)
                {
                    fprintf(fd, "%s,", functions[i].Parameters[j].Type);
                }
            }
            fprintf(fd, "%s);\n", functions[i].Parameters[functions[i].ParameterCount - 1].Type);
        }
        else
            fputs(");\n", fd);
        fprintf(fd, "extern %sType %s;\n", functions[i].Name, functions[i].Name);
    }

    fprintf(fd,
"\n\
//} }\n\
\n\
//#ifndef RF_SHORTHAND_NAMESPACE_OGL\n\
//#define RF_SHORTHAND_NAMESPACE_OGL\n\
//namespace RF_GL = RadonFramework::GL;\n\
//#endif\n\
\n\
#endif // %s", guard);
}

void writeFunctionsSource(FILE* fd, Function* functions, unsigned int functionCount,
    const char* header)
{
    RF_Type::Size i;
    fprintf(fd,
"#include <GL/%s.hpp>\n\
\
namespace RadonFramework { namespace GL {\n\
\n", header);

    for(i = 0; i < functionCount; ++i)
    {
        fprintf(fd, "%sType %s = 0;\n", functions[i].Name, functions[i].Name);
    }

    fputs("\n} }\n", fd);
}

void writeHeader(FILE* fd, Function* functions, unsigned int functionCount)
{
    unsigned int i = 0;
    unsigned int regcount = 0;

    fputs(
"#ifndef RF_DRAWING_OPENGLMACHINE_HPP\n\
#define RF_DRAWING_OPENGLMACHINE_HPP\n\
\
namespace RadonFramework { namespace Drawing {\n\
\n\
namespace GLFunctions\n\
{\n\
\tenum Type\n\
\t{\n", fd);

    for (; i < functionCount; ++i)
    {
        bool shouldBeAdded = false;
        for (unsigned int j = 0; j < functions[i].FeatureCount; ++j)
        { 
            if(strcmp(functions[i].Features[j].API, "gl") == 0 &&
               functions[i].Features[j].introduced)
            {
                shouldBeAdded = true;
            }

            if(strcmp(functions[i].Features[j].API, "gl") == 0 &&
               functions[i].Features[j].removed)
            {
                shouldBeAdded = false;
            }
        }

        if (shouldBeAdded)
        {
            regcount = max(functions[i].ParameterCount, regcount);
            fprintf(fd, "\t\t%s,\n", functions[i].Name + 2);
        }                
    }

    fputs(
"\t\tMAX\n\
\t};\n\
}\n\
\n\
namespace GLOpCode\n\
{\n\
\tenum Type\n\
\t{\n", fd);
    // Move value commands
    for (i = 0; i < regcount; ++i)
    {
        if (i != 0)
            fprintf(fd, "\t\tMoveFloat32Reg%d,\n", i);
        else
            fprintf(fd, "\t\tMoveFloat32Reg%d=GLFunctions::MAX,\n", i);
        fprintf(fd, "\t\tMoveFloat64Reg%d,\n", i);
        fprintf(fd, "\t\tMoveInt16Reg%d,\n", i);
        fprintf(fd, "\t\tMoveInt32Reg%d,\n", i);
        fprintf(fd, "\t\tMoveInt64Reg%d,\n", i);
        fprintf(fd, "\t\tMoveUInt16Reg%d,\n", i);
        fprintf(fd, "\t\tMoveUInt32Reg%d,\n", i);
        fprintf(fd, "\t\tMoveUInt64Reg%d,\n", i);
        fprintf(fd, "\t\tMovePtrReg%d,\n", i);
    }

    // Copy value commands
    for(i = 0; i < regcount; ++i)
    {
        fprintf(fd, "\t\tCopyFloat32Reg%d,\n", i);
        fprintf(fd, "\t\tCopyFloat64Reg%d,\n", i);
        fprintf(fd, "\t\tCopyInt16Reg%d,\n", i);
        fprintf(fd, "\t\tCopyInt32Reg%d,\n", i);
        fprintf(fd, "\t\tCopyInt64Reg%d,\n", i);
        fprintf(fd, "\t\tCopyUInt16Reg%d,\n", i);
        fprintf(fd, "\t\tCopyUInt32Reg%d,\n", i);
        fprintf(fd, "\t\tCopyUInt64Reg%d,\n", i);
        fprintf(fd, "\t\tCopyPtrReg%d,\n", i);
    }

    // Addr commands
    for(i = 0; i < regcount; ++i)
    {
        fprintf(fd, "\t\tAddrPtrReg%d,\n", i);
    }

    // Read commands(only result register)
    fprintf(fd, "\t\tRead4Byte,\n");
    fprintf(fd, "\t\tRead8Byte,\n");

    fputs(
"\t\tMAX\n\
\t};\n\
}\n\
\n", fd);

    const int RESULTREGISTER = 1;
    fprintf(fd,
"class OpenGLMachine\n\
{\n\
public:\n\
\tstatic void CallFunction(GLFunctions::Type);\n\
\tstatic void CallOpCode(GLOpCode::Type, RF_Type::UInt8*&);\n\
\tstatic const RF_Type::UInt32 RegisterSize=8;\n\
\tstatic const RF_Type::UInt32 RegisterCount=%d;\n\
\tstatic RF_Type::UInt8 Registers[RegisterCount*RegisterSize];\n\
\tstatic const RF_Type::UInt8 FunctionParameterCount[GLFunctions::MAX];\n\
};\n\n", regcount + RESULTREGISTER);

    if(regcount > 0)
    {
        // Move command
        fputs("template<typename T>\n\
struct GetMoveOpCode\n\
{\n\
\tstatic const GLOpCode::Type COMMAND[OpenGLMachine::RegisterCount];\n\
};\n\
\n\
template<typename T>\n\
const GLOpCode::Type GetMoveOpCode<T>::COMMAND[] = {\n",fd);
        
        for(i = 0; i < regcount-1; ++i)
        {
            fputs("\tGLOpCode::MAX, \n", fd);
        }
        fputs("\tGLOpCode::MAX\n};\n\n", fd);

        fputs("template<typename T>\n\
struct GetMoveOpCodeTrait\n\
{\n\
\tenum\n\
\t{\n\
\t\tSUPPORTED = false\n\
\t};\n\
};\n\n", fd);

        writeTypeTraits(fd, regcount, "RF_Type::Float32", "Float32", "Move");
        writeTypeTraits(fd, regcount, "RF_Type::Float64", "Float64", "Move");
        writeTypeTraits(fd, regcount, "RF_Type::Int16", "Int16", "Move");
        writeTypeTraits(fd, regcount, "RF_Type::Int32", "Int32", "Move");
        writeTypeTraits(fd, regcount, "RF_Type::Int64", "Int64", "Move");
        writeTypeTraits(fd, regcount, "RF_Type::UInt16", "UInt16", "Move");
        writeTypeTraits(fd, regcount, "RF_Type::UInt32", "UInt32", "Move");
        writeTypeTraits(fd, regcount, "RF_Type::UInt64", "UInt64", "Move");
        writeTypeTraits(fd, regcount, "void*", "Ptr", "Move");

        // Copy command
        fputs("template<typename T>\n\
struct GetCopyOpCode\n\
{\n\
\tstatic const GLOpCode::Type COMMAND[OpenGLMachine::RegisterCount];\n\
};\n\
\n\
template<typename T>\n\
const GLOpCode::Type GetCopyOpCode<T>::COMMAND[] = {\n", fd);

        for(i = 0; i < regcount - 1; ++i)
        {
            fputs("\tGLOpCode::MAX, \n", fd);
        }
        fputs("\tGLOpCode::MAX\n};\n\n", fd);

        fputs("template<typename T>\n\
struct GetCopyOpCodeTrait\n\
{\n\
\tenum\n\
\t{\n\
\t\tSUPPORTED = false\n\
\t};\n\
};\n\n", fd);

        writeTypeTraits(fd, regcount, "RF_Type::Float32", "Float32", "Copy");
        writeTypeTraits(fd, regcount, "RF_Type::Float64", "Float64", "Copy");
        writeTypeTraits(fd, regcount, "RF_Type::Int16", "Int16", "Copy");
        writeTypeTraits(fd, regcount, "RF_Type::Int32", "Int32", "Copy");
        writeTypeTraits(fd, regcount, "RF_Type::Int64", "Int64", "Copy");
        writeTypeTraits(fd, regcount, "RF_Type::UInt16", "UInt16", "Copy");
        writeTypeTraits(fd, regcount, "RF_Type::UInt32", "UInt32", "Copy");
        writeTypeTraits(fd, regcount, "RF_Type::UInt64", "UInt64", "Copy");
        writeTypeTraits(fd, regcount, "void*", "Ptr", "Copy");

        // Addr command
        fputs("template<typename T>\n\
struct GetAddrOpCode\n\
{\n\
\tstatic const GLOpCode::Type COMMAND[OpenGLMachine::RegisterCount];\n\
};\n\
\n\
template<typename T>\n\
const GLOpCode::Type GetAddrOpCode<T>::COMMAND[] = {\n", fd);

        for(i = 0; i < regcount - 1; ++i)
        {
            fputs("\tGLOpCode::MAX, \n", fd);
        }
        fputs("\tGLOpCode::MAX\n};\n\n", fd);

        fputs("template<typename T>\n\
struct GetAddrOpCodeTrait\n\
{\n\
\tenum\n\
\t{\n\
\t\tSUPPORTED = false\n\
\t};\n\
};\n\n", fd);

        writeTypeTraits(fd, regcount, "void*", "Ptr", "Addr");

        // Read command
        fputs("template<RF_Type::Size N>\n\
struct GetReadOpCode\n\
{\n\
\tstatic const GLOpCode::Type COMMAND;\n\
};\n\
\n\
template<RF_Type::Size N>\n\
const GLOpCode::Type GetReadOpCode<N>::COMMAND = GLOpCode::MAX;\n\n", fd);

        fputs("template<RF_Type::Size N>\n\
struct GetReadOpCodeTrait\n\
{\n\
\tenum\n\
\t{\n\
\t\tSUPPORTED = false\n\
\t};\n\
};\n\n", fd);

        fputs("template<>\n\
const GLOpCode::Type GetReadOpCode<4>::COMMAND = GLOpCode::Read4Byte;\n\n", fd);
        fputs("template<>\n\
struct GetReadOpCodeTrait<4>\n\
{\n\
\tenum\n\
\t{\n\
\t\tSUPPORTED = true\n\
\t};\n\
};\n\n",fd);

        fputs("template<>\n\
const GLOpCode::Type GetReadOpCode<8>::COMMAND = GLOpCode::Read8Byte;\n\n", fd);
        fputs("template<>\n\
struct GetReadOpCodeTrait<8>\n\
{\n\
\tenum\n\
\t{\n\
\t\tSUPPORTED = true\n\
\t};\n\
};\n\n", fd);
    }
    fputs("}\n}\n#endif // RF_DRAWING_OPENGLMACHINE_HPP", fd);
}

void writeSource(FILE* fd, Function* functions, unsigned int functionCount)
{
    unsigned int i = 0, j = 0, offset = 0, regcount = 0;

    fputs(
"#include <RadonFramework/precompiled.hpp>\n\
#include <RadonFramework/Drawing/OpenGLMachine.hpp>\n\
#include <GL/glew.h>\n\
\n\
using namespace RadonFramework::Drawing;\n\
using namespace RadonFramework::Core::Types;\n\
\n\
UInt8 OpenGLMachine::Registers[OpenGLMachine::RegisterCount*OpenGLMachine::RegisterSize];\n\
const UInt8 OpenGLMachine::FunctionParameterCount[GLFunctions::MAX]={\n", fd);

    for (; i < functionCount; ++i)
    {
        bool shouldBeAdded = false;
        for(unsigned int k = 0; k < functions[i].FeatureCount; ++k)
        {
            if(strcmp(functions[i].Features[k].API, "gl") == 0 &&
                functions[i].Features[k].introduced)
            {
                shouldBeAdded = true;
            }

            if(strcmp(functions[i].Features[k].API, "gl") == 0 &&
                functions[i].Features[k].removed)
            {
                shouldBeAdded = false;
            }
        }

        if(shouldBeAdded)
        {
            regcount = max(functions[i].ParameterCount, regcount);
            if(j == 0)
                fprintf(fd, "\t%d", functions[i].ParameterCount);
            else
                if(j % 25 == 0)
                    fprintf(fd, ",\n\t%d", functions[i].ParameterCount);
                else
                    fprintf(fd, ", %d", functions[i].ParameterCount);
            ++j;
        }
    }

    fputs(
"\n};\n\n\
void OpenGLMachine::CallFunction(GLFunctions::Type ID)\n\
{\n\
\tswitch(ID)\n\
\t{\n", fd);

    for (i = 0; i < functionCount; ++i)
    {
        bool shouldBeAdded = false;
        for(unsigned int k = 0; k < functions[i].FeatureCount; ++k)
        {
            if(strcmp(functions[i].Features[k].API, "gl") == 0 &&
                functions[i].Features[k].introduced)
            {
                shouldBeAdded = true;
            }

            if(strcmp(functions[i].Features[k].API, "gl") == 0 &&
                functions[i].Features[k].removed)
            {
                shouldBeAdded = false;
            }
        }

        if(shouldBeAdded)
        {
            offset = 0;
            fprintf(fd, "\tcase GLFunctions::%s:\n\t\t%s(", functions[i].Name + 2, functions[i].Name);
            for (j = 0; j < functions[i].ParameterCount; ++j)
            {
                String type;
                if (functions[i].Parameters[j].IsConst)
                    type = "const ";
                type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].Type);
                if (functions[i].Parameters[j].PostType)
                    type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].PostType);

                if (j == 0)
                    fprintf(fd, "*reinterpret_cast<%s*>(Registers)", type.c_str());
                else
                    fprintf(fd, ",\n\t\t\t*reinterpret_cast<%s*>(Registers+%d)", type.c_str(), j * 8);
                offset += functions[i].Parameters[j].ByteSize;
            }
            fputs("); break;\n", fd);
        }
    }

    fputs(
"\t}\n\
}\n\
\n\
void OpenGLMachine::CallOpCode(GLOpCode::Type ID, UInt8*& Data)\n\
{\n\
\tswitch(ID)\n\
\t{\n", fd);

    // move case
    for (i = 0; i < regcount; ++i)
    {
        fprintf(fd, "\tcase GLOpCode::MoveFloat32Reg%d: *reinterpret_cast<Float32*>(Registers+%d)=*reinterpret_cast<Float32*>(Data); Data+=4; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::MoveFloat64Reg%d: *reinterpret_cast<Float64*>(Registers+%d)=*reinterpret_cast<Float64*>(Data); Data+=8; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::MoveInt16Reg%d: *reinterpret_cast<Int16*>(Registers+%d)=*reinterpret_cast<Int16*>(Data); Data+=2; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::MoveInt32Reg%d: *reinterpret_cast<Int32*>(Registers+%d)=*reinterpret_cast<Int32*>(Data); Data+=4; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::MoveInt64Reg%d: *reinterpret_cast<Int64*>(Registers+%d)=*reinterpret_cast<Int64*>(Data); Data+=8; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::MoveUInt16Reg%d: *reinterpret_cast<UInt16*>(Registers+%d)=*reinterpret_cast<UInt16*>(Data); Data+=2; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::MoveUInt32Reg%d: *reinterpret_cast<UInt32*>(Registers+%d)=*reinterpret_cast<UInt32*>(Data); Data+=4; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::MoveUInt64Reg%d: *reinterpret_cast<UInt64*>(Registers+%d)=*reinterpret_cast<UInt64*>(Data); Data+=8; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::MovePtrReg%d: *reinterpret_cast<void**>(Registers+%d)=*reinterpret_cast<void**>(Data); Data+=8; break;\n", i, i * 8);
    }

    // copy case
    for(i = 0; i < regcount; ++i)
    {
        fprintf(fd, "\tcase GLOpCode::CopyFloat32Reg%d: *reinterpret_cast<Float32*>(Registers+%d)=*reinterpret_cast<Float32*>(Data-(*reinterpret_cast<Int32*>(Data))); Data+=4; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::CopyFloat64Reg%d: *reinterpret_cast<Float64*>(Registers+%d)=*reinterpret_cast<Float64*>(Data-(*reinterpret_cast<Int32*>(Data))); Data+=4; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::CopyInt16Reg%d: *reinterpret_cast<Int16*>(Registers+%d)=*reinterpret_cast<Int16*>(Data-(*reinterpret_cast<Int32*>(Data))); Data+=4; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::CopyInt32Reg%d: *reinterpret_cast<Int32*>(Registers+%d)=*reinterpret_cast<Int32*>(Data-(*reinterpret_cast<Int32*>(Data))); Data+=4; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::CopyInt64Reg%d: *reinterpret_cast<Int64*>(Registers+%d)=*reinterpret_cast<Int64*>(Data-(*reinterpret_cast<Int32*>(Data))); Data+=4; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::CopyUInt16Reg%d: *reinterpret_cast<UInt16*>(Registers+%d)=*reinterpret_cast<UInt16*>(Data-(*reinterpret_cast<Int32*>(Data))); Data+=4; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::CopyUInt32Reg%d: *reinterpret_cast<UInt32*>(Registers+%d)=*reinterpret_cast<UInt32*>(Data-(*reinterpret_cast<Int32*>(Data))); Data+=4; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::CopyUInt64Reg%d: *reinterpret_cast<UInt64*>(Registers+%d)=*reinterpret_cast<UInt64*>(Data-(*reinterpret_cast<Int32*>(Data))); Data+=4; break;\n", i, i * 8);
        fprintf(fd, "\tcase GLOpCode::CopyPtrReg%d: *reinterpret_cast<void**>(Registers+%d)=*reinterpret_cast<void**>(Data-(*reinterpret_cast<Int32*>(Data))); Data+=4; break;\n", i, i * 8);
    }

    // addr case
    for(i = 0; i < regcount; ++i)
    {
        fprintf(fd, "\tcase GLOpCode::AddrPtrReg%d: *reinterpret_cast<void**>(Registers+%d)=reinterpret_cast<void*>(Data-(*reinterpret_cast<Int32*>(Data))); Data+=4; break;\n", i, i * 8);
    }

    fprintf(fd, "\tcase GLOpCode::Read4Byte: *reinterpret_cast<RF_Type::UInt32*>(Data-(*reinterpret_cast<Int32*>(Data)))=*reinterpret_cast<RF_Type::UInt32*>(Registers+%d); Data+=4; break;\n", regcount * 8);
    fprintf(fd, "\tcase GLOpCode::Read8Byte: *reinterpret_cast<RF_Type::UInt64*>(Data-(*reinterpret_cast<Int32*>(Data)))=*reinterpret_cast<RF_Type::UInt64*>(Registers+%d); Data+=4; break;\n", regcount * 8);
    fputs("\t}\n}", fd);
}

int GenerateVMOGLCode(const String& path, const String& specFilename)
{
    unsigned int functionCount = 0;
    Function* functions = 0;
    unsigned int constantCount = 0;
    Constant* constants = 0;
    FILE* fdHeader = fopen((path + "/OpenGLMachine.hpp").c_str(), "w+");
    FILE* fdSource = fopen((path + "/OpenGLMachine.cpp").c_str(), "w+");
    if(0 == fdHeader || 0 == fdSource)
        return EXIT_FAILURE;

    FILE* fp = fopen((path + specFilename).c_str(), "r");
    if(0 != fp)
    {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char* xml = new char[size];
        fread(xml, 1, size, fp);
        fclose(fp);
        if(readSpecs(xml, size, &functions, &functionCount, &constants, &constantCount) == 0)
        {
            printf("%s\n\tfunction count: %d\n\tconstants count: %d\n", specFilename.c_str(), functionCount, constantCount);

            writeHeader(fdHeader, functions, functionCount);
            writeSource(fdSource, functions, functionCount);

            freeSpecs(&functions, functionCount, &constants, constantCount);
            fclose(fdHeader);
            fclose(fdSource);
        }
        delete[] xml;
    }
    return 0;
}

int GenerateInterfaceCode(const String& Path, const String& SpecFilename, 
    const String& ConstantFilename, const String& FunctionFilename, 
    const String& Guardname)
{
    unsigned int functionCount = 0;
    Function* functions = 0;
    unsigned int constantCount = 0;
    Constant* constants = 0;
    FILE* fdConstantHeader = fopen((Path + "/" + ConstantFilename + ".hpp").c_str(), "w+");
    FILE* fdFunctionHeader = fopen((Path + "/" + FunctionFilename + ".hpp").c_str(), "w+");
    FILE* fdFunctionSource = fopen((Path + "/" + FunctionFilename + ".cpp").c_str(), "w+");
    if(0 == fdConstantHeader || 0 == fdFunctionHeader || 0 == fdFunctionSource)
        return EXIT_FAILURE;

    FILE* fp = fopen((Path + SpecFilename).c_str(), "r");
    if(0 != fp)
    {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char* xml = new char[size];
        fread(xml, 1, size, fp);
        fclose(fp);
        if(readSpecs(xml, size, &functions, &functionCount, &constants, &constantCount) == 0)
        {
            printf("%s\n\tfunction count: %d\n\tconstants count: %d\n", SpecFilename.c_str(), functionCount, constantCount);

            String guardName("RF_GL_");
            guardName += Guardname;
            guardName += "CONSTANTS_HPP";
            writeConstants(fdConstantHeader, constants, constantCount, guardName.c_str());

            guardName = "RF_GL_";
            guardName += Guardname;
            guardName += "FUNCTIONS_HPP";
            writeFunctionsHeader(fdFunctionHeader, functions, functionCount, guardName.c_str());
            writeFunctionsSource(fdFunctionSource, functions, functionCount, FunctionFilename.c_str());

            freeSpecs(&functions, functionCount, &constants, constantCount);
            fclose(fdConstantHeader);
            fclose(fdFunctionHeader);
            fclose(fdFunctionSource);
        }
        delete[] xml;
    }
    return 0;
}

int main()
{
    //initialize all service locator
    RadonFramework::Radon framework;
    //pipe all log messages to console
    Log::AddAppender(AutoPointer<Appender>(new LogConsole));

    String path = Directory::ApplicationDirectory()->Location().Path();

    int result = GenerateVMOGLCode(path, "/gl.xml");
    
    if(0 == result)
    {
        result = GenerateInterfaceCode(path, "/gl.xml", "OpenGLConstants",
            "OpenGLFunctions", "OPENGL");
    }

    if(0 == result)
    {
        result = GenerateInterfaceCode(path, "/wgl.xml", "WindowsOpenGLConstants", 
            "WindowsOpenGLFunctions", "WINDOWSOPENGL");
    }

    if(0 == result)
    {
        result = GenerateInterfaceCode(path, "/glx.xml", "OpenGLX11Constants", 
            "OpenGLX11Functions", "OPENGLX11");
    }

    return result;
}