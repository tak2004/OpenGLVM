#include "precompiled.hpp"
#include <RadonFramework/Radon.hpp>
#include <libSpec.h>

class GeneratorInfo
{
public:
    RF_Type::String HeaderOutputName;
    RF_Type::String SourceOutputName;
    RF_Type::String ConstantsOutputName;
    RF_Type::String ConstantsGuard;
    RF_Type::String FunctionGuard;
    RF_Type::String Location;
    RF_Type::Bool UseExtensions;
    RF_Type::Bool UseFeatures;
};

// void GenerateOpenGLBackendDispatcher(unsigned int functionCount, Function* functions)
// {
//     auto funcFile = RF_IO::Directory::ApplicationDirectory()->SubFile(RF_Type::String("GraphicMachineOpenGLCommands.hpp"));
//     RF_IO::FileStream functionFile;
//     if(functionFile.Open(funcFile->Location(), RF_SysFile::FileAccessMode::Write, RF_SysFile::FileAccessPriority::DelayReadWrite))
//     {
//         functionFile.WriteText(RF_Type::String("#ifndef RF_DRAWING_GRAPHICMACHINEOPENGLCOMMANDS_HPP\n\
// #define RF_DRAWING_GRAPHICMACHINEOPENGLCOMMANDS_HPP\n\
// \n\
// #include <RadonFramework/Drawing/GraphicMachineOpenGLTypes.hpp>\n\
// \n\
// namespace RadonFramework { namespace Drawing {\n\n"));
// 
//         for(auto i = 0; i < functionCount; ++i)
//         {
//             for(auto k = 0; k < functions[i].FeatureCount; ++k)
//             {
//                 if(RF_Type::String::UnsafeStringCreation(functions[i].Features[k].API) == RF_Type::String("gl"))
//                 {
//                     functionFile.WriteText(RF_Type::String::Format(RF_Type::String("struct GM%s\n\
// {\n\
//     static const GraphicMachineFunction GRAPHICMACHINEFUNCTION;\n"), functions[i].Name+2));
//                     for(auto j = 0; j < functions[i].ParameterCount; ++j)
//                     {
//                         RF_Type::String type;
//                         if(functions[i].Parameters[j].IsConst && functions[i].Parameters[j].IsPointer)
//                             type = RF_Type::String("const ");
//                         type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].Type);
//                         if(functions[i].Parameters[j].PostType)
//                             type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].PostType);
//                         functionFile.WriteText(RF_Type::String::Format(RF_Type::String("\t%s %s;\n"), type.c_str(), functions[i].Parameters[j].Name));
//                     }
//                     functionFile.WriteText(RF_Type::String("};\n\n"));
//                     break;
//                 }
//             }
//         }
// 
//         functionFile.WriteText(RF_Type::String("\n} }\n\n\
// #ifndef RF_SHORTHAND_NAMESPACE_DRAW\n\
// #define RF_SHORTHAND_NAMESPACE_DRAW\n\
//     namespace RF_Draw = RadonFramework::Drawing;\n\
// #endif\n\n\
// #endif // RF_DRAWING_GRAPHICMACHINEOPENGLCOMMANDS_HPP"));
//         functionFile.Close();
//     }
// 
//     funcFile = RF_IO::Directory::ApplicationDirectory()->SubFile(RF_Type::String("GraphicMachineOpenGLCommands.cpp"));
//     if(functionFile.Open(funcFile->Location(), RF_SysFile::FileAccessMode::Write, RF_SysFile::FileAccessPriority::DelayReadWrite))
//     {
//         functionFile.WriteText(RF_Type::String("#include \"RadonFramework/precompiled.hpp\"\n\
// #include \"RadonFramework/Drawing/GraphicMachineOpenGLCommands.hpp\"\n\
// #include \"RadonFramework/backend/GL/glew.h\"\n\
// \n\
// namespace RadonFramework { namespace Drawing {\n\n"));
// 
//         for(auto i = 0; i < functionCount; ++i)
//         {
//             for(auto k = 0; k < functions[i].FeatureCount; ++k)
//             {
//                 if(RF_Type::String::UnsafeStringCreation(functions[i].Features[k].API) == "gl")
//                 {
//                     functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %sFunction(void* Data){\n\
//     GM%s* data = reinterpret_cast<GM%s*>(Data);\n"), functions[i].Result, functions[i].Name+2, functions[i].Name + 2, functions[i].Name + 2));
// 
//                     auto offset = 0;
//                     if(RF_Type::String::UnsafeStringCreation(functions[i].Result) == RF_Type::String("void"))
//                     {
//                         functionFile.WriteText(RF_Type::String::Format(RF_Type::String("\t%s("), functions[i].Name));
//                     }
//                     else
//                     {
//                         functionFile.WriteText(RF_Type::String::Format(RF_Type::String("\treturn %s("), functions[i].Name));
//                     }
// 
//                     for(auto j = 0; j < functions[i].ParameterCount; ++j)
//                     {
//                         if(j == 0)
//                         {
//                             functionFile.WriteText(RF_Type::String::Format(RF_Type::String("data->%s"), functions[i].Parameters[j].Name));
//                         }
//                         else
//                         {
//                             functionFile.WriteText(RF_Type::String::Format(RF_Type::String(", data->%s"), functions[i].Parameters[j].Name));
//                         }
//                         offset += functions[i].Parameters[j].ByteSize;
//                     }
//                     functionFile.WriteText(RF_Type::String::Format(RF_Type::String(");\n\
// }\n\
// const GraphicMachineFunction GM%s::GRAPHICMACHINEFUNCTION = reinterpret_cast<GraphicMachineFunction>(&%sFunction);\n\
// \n"), functions[i].Name + 2, functions[i].Name + 2));
//                     break;
//                 }
//             }
//         }
//         functionFile.WriteText(RF_Type::String("} }"));
//         functionFile.Close();
//     }
// }
// 
// void GenerateFunctions(unsigned int functionCount, Function* functions)
// {
//     auto funcFile = RF_IO::Directory::ApplicationDirectory()->SubFile(RF_Type::String("GraphicMachineOpenGLFunctions.hpp"));
//     RF_IO::FileStream functionFile;
//     if(functionFile.Open(funcFile->Location(), RF_SysFile::FileAccessMode::Write, RF_SysFile::FileAccessPriority::DelayReadWrite))
//     {
//         functionFile.WriteText(RF_Type::String("#ifndef RF_DRAWING_GRAPHICMACHINEOPENGLFUNCTIONS_HPP\n\
// #define RF_DRAWING_GRAPHICMACHINEOPENGLFUNCTIONS_HPP\n\n\
// #include <RadonFramework/Drawing/GraphicMachineOpenGLTypes.hpp>\n\n"));
// 
//         for(auto i = 0; i < functionCount; ++i)
//         {
//             for(auto k = 0; k < functions[i].FeatureCount; ++k)
//             {
//                 if(RF_Type::String::UnsafeStringCreation(functions[i].Features[k].API) == RF_Type::String("gl"))
//                 {
//                     functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %s("), functions[i].Result, functions[i].Name));
//                     if(functions[i].ParameterCount > 0)
//                     {
//                         if(functions[i].ParameterCount > 1)
//                         {
//                             for(auto j = 0; j < functions[i].ParameterCount - 1; ++j)
//                             {
//                                 RF_Type::String type;
//                                 if(functions[i].Parameters[j].IsConst)
//                                     type = RF_Type::String("const ");
//                                 type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].Type);
//                                 if(functions[i].Parameters[j].PostType)
//                                     type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].PostType);
//                                 functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %s,"), type.c_str(), functions[i].Parameters[j].Name));
//                             }
//                         }
//                         RF_Type::String type;
//                         if(functions[i].Parameters[functions[i].ParameterCount - 1].IsConst)
//                             type = RF_Type::String("const ");
//                         type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[functions[i].ParameterCount - 1].Type);
//                         if(functions[i].Parameters[functions[i].ParameterCount - 1].PostType)
//                             type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[functions[i].ParameterCount - 1].PostType);
// 
//                         functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %s);\n"), type.c_str(), functions[i].Parameters[functions[i].ParameterCount - 1].Name));
//                     }
//                     else
//                         functionFile.WriteText(RF_Type::String::Format(RF_Type::String(");\n")));
//                     break;
//                 }
//             }
//         }
// 
//         functionFile.WriteText(RF_Type::String("\n\
// #endif // RF_DRAWING_GRAPHICMACHINEOPENGLFUNCTIONS_HPP"));
//         functionFile.Close();
//     }
// 
//     funcFile = RF_IO::Directory::ApplicationDirectory()->SubFile(RF_Type::String("GraphicMachineOpenGLFunctions.cpp"));
//     if(functionFile.Open(funcFile->Location(), RF_SysFile::FileAccessMode::Write, RF_SysFile::FileAccessPriority::DelayReadWrite))
//     {
//         functionFile.WriteText(RF_Type::String("#include \"RadonFramework/Drawing/GraphicMachineOpenGLFunctions.hpp\"\n\
// #include \"RadonFramework/Drawing/GraphicMachineCommandBuffer.hpp\"\n\
// #include \"RadonFramework/Drawing/GraphicMachineOpenGLCommands.hpp\"\n\n"));
// 
//         for(auto i = 0; i < functionCount; ++i)
//         {
//             for(auto k = 0; k < functions[i].FeatureCount; ++k)
//             {
//                 if(RF_Type::String::UnsafeStringCreation(functions[i].Features[k].API) == RF_Type::String("gl"))
//                 {
//                     functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %s("), functions[i].Result, functions[i].Name));
//                     if(functions[i].ParameterCount > 0)
//                     {
//                         if(functions[i].ParameterCount > 1)
//                         {
//                             for(auto j = 0; j < functions[i].ParameterCount - 1; ++j)
//                             {
//                                 RF_Type::String type;
//                                 if(functions[i].Parameters[j].IsConst)
//                                     type = "const ";
//                                 type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].Type);
//                                 if(functions[i].Parameters[j].PostType)
//                                     type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].PostType);
//                                 functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %s,"), type.c_str(), functions[i].Parameters[j].Name));
//                             }
//                         }
//                         RF_Type::String type;
//                         if(functions[i].Parameters[functions[i].ParameterCount - 1].IsConst)
//                             type = "const ";
//                         type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[functions[i].ParameterCount - 1].Type);
//                         if(functions[i].Parameters[functions[i].ParameterCount - 1].PostType)
//                             type += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[functions[i].ParameterCount - 1].PostType);
//                         functionFile.WriteText(RF_Type::String::Format(RF_Type::String("%s %s){\n"), type.c_str(), functions[i].Parameters[functions[i].ParameterCount - 1].Name));
//                     }
//                     else
//                         functionFile.WriteText(RF_Type::String::Format(RF_Type::String("){\n")));
//                     functionFile.WriteText(RF_Type::String::Format(RF_Type::String("\
//     auto* cmdBuffer = RF_Draw::GraphicMachineCommandBuffer::GetThreadInstance();\n\
//     RF_Draw::GM%s* cmd = cmdBuffer->AddCommand<RF_Draw::GM%s>();\n"), functions[i].Name + 2, functions[i].Name + 2));
// 
//                     for(auto j = 0; j < functions[i].ParameterCount; ++j)
//                     {
//                         functionFile.WriteText(RF_Type::String::Format(RF_Type::String("\
//     cmd->%s = %s;\n"), 
//     functions[i].Parameters[j].Name, functions[i].Parameters[j].Name));
//                     }
// 
//                     functionFile.WriteText(RF_Type::String("}\n\n"));
//                     break;
//                 }
//             }
//         }
//         functionFile.Close();
//     }
// }

void GenerateConstants(unsigned int constantCount, Constant* constants, GeneratorInfo& Info)
{
    auto constFile = RF_IO::Directory::ApplicationDirectory()->SubFile(Info.ConstantsOutputName);
    RF_IO::FileStream constantFile;
    if(constantFile.Open(constFile->Location(), RF_SysFile::FileAccessMode::Write, RF_SysFile::FileAccessPriority::DelayReadWrite))
    {
        constantFile.WriteText(RF_Type::String::Format("#ifndef %s\n\
#define %s\n\
#if _MSC_VER > 1000\n\
#pragma once\n\
#endif\n\n"_rfs, Info.ConstantsGuard.c_str(), Info.ConstantsGuard.c_str()));

        for(auto i = 0; i < constantCount; ++i)
        {
            if(constants[i].API != 0)
            {
                auto api = RF_Type::String::UnsafeStringCreation(constants[i].API);
                if (api == "gl"_rfs || api == "wgl"_rfs || api == "glx"_rfs)
                {
                    constantFile.WriteText(RF_Type::String::Format(RF_Type::String("#define %s %s\n"), constants[i].Name, constants[i].Value));
                }
            }
        }

        constantFile.WriteText(RF_Type::String::Format("\n#endif // %s"_rfs, Info.ConstantsGuard.c_str()));
        constantFile.Close();
    }
}


void GenerateOpenGLFunctionDispatcher(RF_Type::UInt32 functionCount, Function* functions,
    GeneratorInfo& Infos)
{
    auto funcFile = RF_IO::Directory::ApplicationDirectory()->SubFile(Infos.HeaderOutputName);
    RF_IO::FileStream functionFile;
    if(functionFile.Open(funcFile->Location(), RF_SysFile::FileAccessMode::Write, RF_SysFile::FileAccessPriority::DelayReadWrite))
    {
        functionFile.WriteText(RF_Type::String::Format("#ifndef %s\n\
#define %s\n\
#if _MSC_VER > 1000\n\
#pragma once\n\
#endif\n\n\
#include <%sOpenGLTypes.hpp>\n\
#include <%s%s>\n\n"_rfs, Infos.FunctionGuard.c_str(), Infos.FunctionGuard.c_str(), Infos.Location.c_str(),
    Infos.Location.c_str(), Infos.ConstantsOutputName.c_str()));

        for(auto i = 0; i < functionCount; ++i)
        {
            RF_Type::Bool pickup = false;

            if (Infos.UseFeatures)
            {
                for(auto k = 0; k < functions[i].FeatureCount; ++k)
                {
                    if(functions[i].Features[k].API != 0)
                    {
                        auto api = RF_Type::String::UnsafeStringCreation(functions[i].Features[k].API);
                        if(api == "gl"_rfs || api == "wgl"_rfs || api == "glx"_rfs)
                        {
                            pickup = true;
                            break;
                        }
                    }
                }
            }

            if (Infos.UseExtensions)
            {
                for(auto k = 0; k < functions[i].ExtensionCount; ++k)
                {
                    if(functions[i].Extensions[k].Supported != 0)
                    {
                        auto apis = RF_Type::String::UnsafeStringCreation(functions[i].Extensions[k].Supported).Split("|"_rfs);
                        
                        if(RF_Algo::Exists(apis, [](RF_Mem::AutoPointerArray<RF_Type::String>::ConstEnumeratorType& Element) {
                            return (*Element == "gl"_rfs) || (*Element == "wgl"_rfs) || (*Element == "glx"_rfs);
                        }))
                        {
                            pickup = true;
                            break;
                        }
                    }
                }
            }

            if (pickup)
            {
                functionFile.WriteText(RF_Type::String::Format("using %sCallback = %s (*)("_rfs, functions[i].Name, functions[i].Result));
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
            }
        }

        functionFile.WriteText(RF_Type::String::Format("\n\
#endif // %s"_rfs, Infos.FunctionGuard.c_str()));
        functionFile.Close();
    }

    funcFile = RF_IO::Directory::ApplicationDirectory()->SubFile(Infos.SourceOutputName);
    if(functionFile.Open(funcFile->Location(), RF_SysFile::FileAccessMode::Write, RF_SysFile::FileAccessPriority::DelayReadWrite))
    {
        functionFile.WriteText(RF_Type::String::Format("#include \"%s%s\"\n\
#include \"%sOpenGLSystem.hpp\"\n\n"_rfs, Infos.Location.c_str(), Infos.HeaderOutputName.c_str(), Infos.Location.c_str()));

        for(auto i = 0; i < functionCount; ++i)
        {
            for(auto k = 0; k < functions[i].FeatureCount; ++k)
            {
                if(functions[i].Features[k].API != 0)
                {
                    auto api = RF_Type::String::UnsafeStringCreation(functions[i].Features[k].API);
                    if(api == "gl"_rfs || api == "wgl"_rfs || api == "glx"_rfs)
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

                        functionFile.WriteText(RF_Type::String::Format("\t%s = reinterpret_cast<%sCallback>(OpenGLGetProcAddress(\"%s\"));\n"_rfs, 
                            functions[i].Name, functions[i].Name, functions[i].Name));
                        if(RF_Type::String::UnsafeStringCreation(functions[i].Result) != "void"_rfs)
                        {
                            functionFile.WriteText("\treturn "_rfs);
                        }                    
                        RF_Type::String params;
                        for(auto j = 0; j < functions[i].ParameterCount; ++j)
                        {
                            if (j>0)
                                params += ","_rfs + RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].Name);
                            else
                                params += RF_Type::String::UnsafeStringCreation(functions[i].Parameters[j].Name);
                        }
                        functionFile.WriteText(RF_Type::String::Format("\t%s(%s);\n\
}\n\
%sCallback %s = %sDispatcher;\n\n"_rfs, functions[i].Name, params.c_str(),
    functions[i].Name, functions[i].Name, functions[i].Name));

                        break;
                    }
                }
            }
        }
        functionFile.Close();
    }
}

void GenerateOpenGLHeaders()
{
    RF_Type::String sources[] = {"gl.xml"_rfs , "wgl.xml"_rfs, "glx.xml"_rfs};
    GeneratorInfo generatorInfos[] = {
        {
            "OpenGL.hpp"_rfs , "OpenGL.cpp"_rfs, "OpenGLConstants.hpp"_rfs,
            "RF_SYSTEM_DRAWING_OPENGLCONSTANTS_HPP"_rfs,
            "RF_SYSTEM_DRAWING_OPENGL_HPP"_rfs, "RadonFramework/System/Drawing/"_rfs,
            true, true
        },
        {
            "WindowsOpenGL.hpp"_rfs, "WindowsOpenGL.cpp"_rfs,
            "WindowsOpenGLConstants.hpp"_rfs, "RF_SYSTEM_DRAWING_WINDOWSOPENGLCONSTANTS_HPP"_rfs,
            "RF_SYSTEM_DRAWING_WINDOWSOPENGL_HPP"_rfs, "RadonFramework/System/Drawing/"_rfs,
            true, false
        },
        {
            "OpenGLX.hpp"_rfs, "OpenGLX.cpp"_rfs, "OpenGLXConstants.hpp"_rfs,
            "RF_SYSTEM_DRAWING_OPENGLXCONSTANTS_HPP"_rfs, "RF_SYSTEM_DRAWING_OPENGLX_HPP"_rfs,
            "RadonFramework/System/Drawing/"_rfs,
            true, false
        }
    };

    for (RF_Type::Size i = 0; i < 3; ++i)
    {
        auto spec = RF_IO::Directory::ApplicationDirectory()->SubFile(sources[i]);
        if(spec && spec->Exists())
        {
            auto data = spec->Read();
            unsigned int functionCount = 0;
            Function* functions = 0;
            unsigned int constantCount = 0;
            Constant* constants = 0;

            if(readSpecs(reinterpret_cast<char*>(data.Get()), data.Size(), &functions, &functionCount, &constants, &constantCount) == 0)
            {
                GenerateConstants(constantCount, constants, generatorInfos[i]);
                GenerateOpenGLFunctionDispatcher(functionCount, functions, generatorInfos[i]);

                freeSpecs(&functions, functionCount, &constants, constantCount);
            }
        }
    }
}