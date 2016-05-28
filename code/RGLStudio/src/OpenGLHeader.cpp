#include "precompiled.hpp"
#include <RadonFramework/Radon.hpp>
#include <libSpec.h>

void GenerateOpenGLBackendDispatcher(unsigned int functionCount, Function* functions)
{
    auto funcFile = RF_IO::Directory::ApplicationDirectory()->SubFile(RF_Type::String("GraphicMachineOpenGLCommands.hpp"));
    RF_IO::FileStream functionFile;
    if(functionFile.Open(funcFile->Location(), RF_SysFile::FileAccessMode::Write, RF_SysFile::FileAccessPriority::DelayReadWrite))
    {
        functionFile.WriteText(RF_Type::String("#ifndef RF_DRAWING_GRAPHICMACHINEOPENGLCOMMANDS_HPP\n\
#define RF_DRAWING_GRAPHICMACHINEOPENGLCOMMANDS_HPP\n\
\n\
#include <RadonFramework/Drawing/GraphicMachineOpenGLTypes.hpp>\n\
\n\
namespace RadonFramework { namespace Drawing {\n\n"));

        for(auto i = 0; i < functionCount; ++i)
        {
            for(auto k = 0; k < functions[i].FeatureCount; ++k)
            {
                if(RF_Type::String::UnsafeStringCreation(functions[i].Features[k].API) == RF_Type::String("gl"))
                {
                    functionFile.WriteText(RF_Type::String::Format(RF_Type::String("struct GM%s\n\
{\n\
    static const GraphicMachineFunction GRAPHICMACHINEFUNCTION;\n"), functions[i].Name+2));
                    for(auto j = 0; j < functions[i].ParameterCount; ++j)
                    {
                        RF_Type::String type;
                        if(functions[i].Parameters[j].IsConst && functions[i].Parameters[j].IsPointer)
                            type = RF_Type::String("const ");
                        type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].Type);
                        if(functions[i].Parameters[j].PostType)
                            type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].PostType);
                        functionFile.WriteText(RF_Type::String::Format(RF_Type::String("\t%s %s;\n"), type.c_str(), functions[i].Parameters[j].Name));
                    }
                    functionFile.WriteText(RF_Type::String("};\n\n"));
                    break;
                }
            }
        }

        functionFile.WriteText(RF_Type::String("\n} }\n\n\
#ifndef RF_SHORTHAND_NAMESPACE_DRAW\n\
#define RF_SHORTHAND_NAMESPACE_DRAW\n\
    namespace RF_Draw = RadonFramework::Drawing;\n\
#endif\n\n\
#endif // RF_DRAWING_GRAPHICMACHINEOPENGLCOMMANDS_HPP"));
        functionFile.Close();
    }

    funcFile = RF_IO::Directory::ApplicationDirectory()->SubFile(RF_Type::String("GraphicMachineOpenGLCommands.cpp"));
    if(functionFile.Open(funcFile->Location(), RF_SysFile::FileAccessMode::Write, RF_SysFile::FileAccessPriority::DelayReadWrite))
    {
        functionFile.WriteText(RF_Type::String("#include \"RadonFramework/precompiled.hpp\"\n\
#include \"RadonFramework/Drawing/GraphicMachineOpenGLCommands.hpp\"\n\
#include \"RadonFramework/backend/GL/glew.h\"\n\
\n\
namespace RadonFramework { namespace Drawing {\n\n"));

        for(auto i = 0; i < functionCount; ++i)
        {
            for(auto k = 0; k < functions[i].FeatureCount; ++k)
            {
                if(RF_Type::String::UnsafeStringCreation(functions[i].Features[k].API) == "gl")
                {
                    functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %sFunction(void* Data){\n\
    GM%s* data = reinterpret_cast<GM%s*>(Data);\n"), functions[i].Result, functions[i].Name+2, functions[i].Name + 2, functions[i].Name + 2));

                    auto offset = 0;
                    if(RF_Type::String::UnsafeStringCreation(functions[i].Result) == RF_Type::String("void"))
                    {
                        functionFile.WriteText(RF_Type::String::Format(RF_Type::String("\t%s("), functions[i].Name));
                    }
                    else
                    {
                        functionFile.WriteText(RF_Type::String::Format(RF_Type::String("\treturn %s("), functions[i].Name));
                    }

                    for(auto j = 0; j < functions[i].ParameterCount; ++j)
                    {
                        if(j == 0)
                        {
                            functionFile.WriteText(RF_Type::String::Format(RF_Type::String("data->%s"), functions[i].Parameters[j].Name));
                        }
                        else
                        {
                            functionFile.WriteText(RF_Type::String::Format(RF_Type::String(", data->%s"), functions[i].Parameters[j].Name));
                        }
                        offset += functions[i].Parameters[j].ByteSize;
                    }
                    functionFile.WriteText(RF_Type::String::Format(RF_Type::String(");\n\
}\n\
const GraphicMachineFunction GM%s::GRAPHICMACHINEFUNCTION = reinterpret_cast<GraphicMachineFunction>(&%sFunction);\n\
\n"), functions[i].Name + 2, functions[i].Name + 2));
                    break;
                }
            }
        }
        functionFile.WriteText(RF_Type::String("} }"));
        functionFile.Close();
    }
}

void GenerateConstants(unsigned int constantCount, Constant* constants)
{
    auto constFile = RF_IO::Directory::ApplicationDirectory()->SubFile(RF_Type::String("GraphicMachineOpenGLConstants.hpp"));
    RF_IO::FileStream constantFile;
    if(constantFile.Open(constFile->Location(), RF_SysFile::FileAccessMode::Write, RF_SysFile::FileAccessPriority::DelayReadWrite))
    {
        constantFile.WriteText(RF_Type::String("#ifndef RF_DRAWING_GRAPHICMACHINEOPENGLCONSTANTS_HPP\n\
#define RF_DRAWING_GRAPHICMACHINEOPENGLCONSTANTS_HPP\n\
\n"));

        for(auto i = 0; i < constantCount; ++i)
        {
            if(constants[i].API == 0 ||
               RF_Type::String::UnsafeStringCreation(constants[i].API) == RF_Type::String("gl"))
            {
                constantFile.WriteText(RF_Type::String::Format(RF_Type::String("#define %s %s\n"), constants[i].Name, constants[i].Value));
            }
        }

        constantFile.WriteText(RF_Type::String("\n#endif // RF_DRAWING_GRAPHICMACHINEOPENGLCONSTANTS_HPP"));
        constantFile.Close();
    }
}

void GenerateFunctions(unsigned int functionCount, Function* functions)
{
    auto funcFile = RF_IO::Directory::ApplicationDirectory()->SubFile(RF_Type::String("GraphicMachineOpenGLFunctions.hpp"));
    RF_IO::FileStream functionFile;
    if(functionFile.Open(funcFile->Location(), RF_SysFile::FileAccessMode::Write, RF_SysFile::FileAccessPriority::DelayReadWrite))
    {
        functionFile.WriteText(RF_Type::String("#ifndef RF_DRAWING_GRAPHICMACHINEOPENGLFUNCTIONS_HPP\n\
#define RF_DRAWING_GRAPHICMACHINEOPENGLFUNCTIONS_HPP\n\n\
#include <RadonFramework/Drawing/GraphicMachineOpenGLTypes.hpp>\n\n"));

        for(auto i = 0; i < functionCount; ++i)
        {
            for(auto k = 0; k < functions[i].FeatureCount; ++k)
            {
                if(RF_Type::String::UnsafeStringCreation(functions[i].Features[k].API) == RF_Type::String("gl"))
                {
                    functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %s("), functions[i].Result, functions[i].Name));
                    if(functions[i].ParameterCount > 0)
                    {
                        if(functions[i].ParameterCount > 1)
                        {
                            for(auto j = 0; j < functions[i].ParameterCount - 1; ++j)
                            {
                                RF_Type::String type;
                                if(functions[i].Parameters[j].IsConst)
                                    type = RF_Type::String("const ");
                                type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].Type);
                                if(functions[i].Parameters[j].PostType)
                                    type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].PostType);
                                functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %s,"), type.c_str(), functions[i].Parameters[j].Name));
                            }
                        }
                        RF_Type::String type;
                        if(functions[i].Parameters[functions[i].ParameterCount - 1].IsConst)
                            type = RF_Type::String("const ");
                        type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[functions[i].ParameterCount - 1].Type);
                        if(functions[i].Parameters[functions[i].ParameterCount - 1].PostType)
                            type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[functions[i].ParameterCount - 1].PostType);

                        functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %s);\n"), type.c_str(), functions[i].Parameters[functions[i].ParameterCount - 1].Name));
                    }
                    else
                        functionFile.WriteText(RF_Type::String::Format(RF_Type::String(");\n")));
                    break;
                }
            }
        }

        functionFile.WriteText(RF_Type::String("\n\
#endif // RF_DRAWING_GRAPHICMACHINEOPENGLFUNCTIONS_HPP"));
        functionFile.Close();
    }

    funcFile = RF_IO::Directory::ApplicationDirectory()->SubFile(RF_Type::String("GraphicMachineOpenGLFunctions.cpp"));
    if(functionFile.Open(funcFile->Location(), RF_SysFile::FileAccessMode::Write, RF_SysFile::FileAccessPriority::DelayReadWrite))
    {
        functionFile.WriteText(RF_Type::String("#include \"RadonFramework/Drawing/GraphicMachineOpenGLFunctions.hpp\"\n\
#include \"RadonFramework/Drawing/GraphicMachineCommandBuffer.hpp\"\n\
#include \"RadonFramework/Drawing/GraphicMachineOpenGLCommands.hpp\"\n\n"));

        for(auto i = 0; i < functionCount; ++i)
        {
            for(auto k = 0; k < functions[i].FeatureCount; ++k)
            {
                if(RF_Type::String::UnsafeStringCreation(functions[i].Features[k].API) == RF_Type::String("gl"))
                {
                    functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %s("), functions[i].Result, functions[i].Name));
                    if(functions[i].ParameterCount > 0)
                    {
                        if(functions[i].ParameterCount > 1)
                        {
                            for(auto j = 0; j < functions[i].ParameterCount - 1; ++j)
                            {
                                RF_Type::String type;
                                if(functions[i].Parameters[j].IsConst)
                                    type = "const ";
                                type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].Type);
                                if(functions[i].Parameters[j].PostType)
                                    type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].PostType);
                                functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %s,"), type.c_str(), functions[i].Parameters[j].Name));
                            }
                        }
                        RF_Type::String type;
                        if(functions[i].Parameters[functions[i].ParameterCount - 1].IsConst)
                            type = "const ";
                        type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[functions[i].ParameterCount - 1].Type);
                        if(functions[i].Parameters[functions[i].ParameterCount - 1].PostType)
                            type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[functions[i].ParameterCount - 1].PostType);
                        functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %s){\n"), type.c_str(), functions[i].Parameters[functions[i].ParameterCount - 1].Name));
                    }
                    else
                        functionFile.WriteText(RF_Type::String::Format(RF_Type::String("){\n")));
                    functionFile.WriteText(RF_Type::String::Format(RF_Type::String("\
    auto* cmdBuffer = RF_Draw::GraphicMachineCommandBuffer::GetThreadInstance();\n\
    RF_Draw::GM%s* cmd = cmdBuffer->AddCommand<RF_Draw::GM%s>();\n"), functions[i].Name + 2, functions[i].Name + 2));

                    for(auto j = 0; j < functions[i].ParameterCount; ++j)
                    {
                        functionFile.WriteText(RF_Type::String::Format(RF_Type::String("\
    cmd->%s = %s;\n"), 
    functions[i].Parameters[j].Name, functions[i].Parameters[j].Name));
                    }

                    functionFile.WriteText(RF_Type::String("}\n\n"));
                    break;
                }
            }
        }
        functionFile.Close();
    }
}

void GenerateOpenGLFunctionDispatcher(RF_Type::UInt32 functionCount, Function* functions)
{
    auto funcFile = RF_IO::Directory::ApplicationDirectory()->SubFile(RF_Type::String("OpenGL.hpp"));
    RF_IO::FileStream functionFile;
    if(functionFile.Open(funcFile->Location(), RF_SysFile::FileAccessMode::Write, RF_SysFile::FileAccessPriority::DelayReadWrite))
    {
        functionFile.WriteText("#ifndef RF_SYSTEM_DRAWING_OPENGL_HPP\n\
#define RF_SYSTEM_DRAWING_OPENGL_HPP\n\n"_rfs);

        for(auto i = 0; i < functionCount; ++i)
        {
            for(auto k = 0; k < functions[i].FeatureCount; ++k)
            {
                if(RF_Type::String::UnsafeStringCreation(functions[i].Features[k].API) == "gl"_rfs)
                {
                    functionFile.WriteText(RF_Type::String::Format("%s (*%sCallback)("_rfs, functions[i].Result, functions[i].Name));
                    if(functions[i].ParameterCount > 0)
                    {
                        if(functions[i].ParameterCount > 1)
                        {
                            for(auto j = 0; j < functions[i].ParameterCount - 1; ++j)
                            {
                                RF_Type::String type;
                                if(functions[i].Parameters[j].IsConst)
                                    type = "const "_rfs;
                                type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].Type);
                                if(functions[i].Parameters[j].PostType)
                                    type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].PostType);
                                functionFile.WriteText(RF_Type::String::Format("%s %s,"_rfs, type.c_str(), functions[i].Parameters[j].Name));
                            }
                        }
                        RF_Type::String type;
                        if(functions[i].Parameters[functions[i].ParameterCount - 1].IsConst)
                            type = "const "_rfs;
                        type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[functions[i].ParameterCount - 1].Type);
                        if(functions[i].Parameters[functions[i].ParameterCount - 1].PostType)
                            type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[functions[i].ParameterCount - 1].PostType);

                        functionFile.WriteText(RF_Type::String::Format("%s %s);\n"_rfs, type.c_str(), functions[i].Parameters[functions[i].ParameterCount - 1].Name));
                    }
                    else
                        functionFile.WriteText(");\n"_rfs);
                    functionFile.WriteText(RF_Type::String::Format("extern %sCallback %s;\n"_rfs, functions[i].Name, functions[i].Name));
                    break;
                }
            }
        }

        functionFile.WriteText(RF_Type::String("\n\
#endif // RF_SYSTEM_DRAWING_OPENGL_HPP"));
        functionFile.Close();
    }

    funcFile = RF_IO::Directory::ApplicationDirectory()->SubFile(RF_Type::String("OpenGL.cpp"));
    if(functionFile.Open(funcFile->Location(), RF_SysFile::FileAccessMode::Write, RF_SysFile::FileAccessPriority::DelayReadWrite))
    {
        functionFile.WriteText("#include \"RadonFramework/System/Drawing/OpenGL.hpp\"\n\n\
    "_rfs);

        for(auto i = 0; i < functionCount; ++i)
        {
            for(auto k = 0; k < functions[i].FeatureCount; ++k)
            {
                if(RF_Type::String::UnsafeStringCreation(functions[i].Features[k].API) == "gl"_rfs)
                {

                    functionFile.WriteText(RF_Type::String::Format("%s %sDispatcher("_rfs, functions[i].Result, functions[i].Name));
                    if(functions[i].ParameterCount > 0)
                    {
                        if(functions[i].ParameterCount > 1)
                        {
                            for(auto j = 0; j < functions[i].ParameterCount - 1; ++j)
                            {
                                RF_Type::String type;
                                if(functions[i].Parameters[j].IsConst)
                                    type = "const "_rfs;
                                type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].Type);
                                if(functions[i].Parameters[j].PostType)
                                    type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].PostType);
                                functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %s,"), type.c_str(), functions[i].Parameters[j].Name));
                            }
                        }
                        RF_Type::String type;
                        if(functions[i].Parameters[functions[i].ParameterCount - 1].IsConst)
                            type = "const "_rfs;
                        type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[functions[i].ParameterCount - 1].Type);
                        if(functions[i].Parameters[functions[i].ParameterCount - 1].PostType)
                            type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[functions[i].ParameterCount - 1].PostType);
                        functionFile.WriteText(RF_Type::String::Format("%s %s){\n"_rfs, type.c_str(), functions[i].Parameters[functions[i].ParameterCount - 1].Name));
                    }
                    else
                        functionFile.WriteText(RF_Type::String::Format("){\n"_rfs));

                    functionFile.WriteText(RF_Type::String::Format("\
    auto* cmdBuffer = RF_Draw::GraphicMachineCommandBuffer::GetThreadInstance();\n\
    RF_Draw::GM%s* cmd = cmdBuffer->AddCommand<RF_Draw::GM%s>();\n"_rfs, functions[i].Name + 2, functions[i].Name + 2));

                    for(auto j = 0; j < functions[i].ParameterCount; ++j)
                    {
                        functionFile.WriteText(RF_Type::String::Format(RF_Type::String("\
    cmd->%s = %s;\n"),
                                                                       functions[i].Parameters[j].Name, functions[i].Parameters[j].Name));
                    }

                    functionFile.WriteText(RF_Type::String("}\n\n"));
                    break;
                }
            }
        }
        functionFile.Close();
    }
}

void GenerateOpenGLHeaders()
{
    auto spec = RF_IO::Directory::ApplicationDirectory()->SubFile(RF_Type::String("gl.xml"));
    if (spec && spec->Exists())
    {
        auto data = spec->Read();
        unsigned int functionCount = 0;
        Function* functions = 0;
        unsigned int constantCount = 0;
        Constant* constants = 0;

        if(readSpecs(reinterpret_cast<char*>(data.Get()), data.Size(), &functions, &functionCount, &constants, &constantCount) == 0)
        {
            GenerateConstants(constantCount, constants);
            GenerateFunctions(functionCount, functions);
            GenerateOpenGLBackendDispatcher(functionCount, functions);
            GenerateOpenGLFunctionDispatcher(functionCount, functions);

            freeSpecs(&functions, functionCount, &constants, constantCount);
        }
    }
}