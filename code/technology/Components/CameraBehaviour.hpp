#ifndef COMPONENTS_CAMERABEHAVIOUR_HPP
#define COMPONENTS_CAMERABEHAVIOUR_HPP
#if _MSC_VER > 1000
#pragma once
#endif

#include <RFECS/BaseComponent.hpp>
#include <RFECS/SharedAttributePool.hpp>
#include "Components/Identifiers.hpp"
#include "SharedAttributes/Identifiers.hpp"
#include "SharedAttributes/Transform.hpp"
#include "RadonFramework/Math/Geometry/ProjectionMatrix.hpp"
#include "Components/RendererMessage.hpp"

namespace Technology { namespace Components {

    typedef RFECS::BaseComponent<Identifiers::CameraBehaviour> CameraBehaviour;

} }

namespace RadonFastEntityComponentSystem{

template<>
struct BaseComponent<Comp::Identifiers::CameraBehaviour>
{
    BaseComponent()
    :m_Entity(0)
    ,m_ViewType(RF_Geo::Viewtype::View3D)
    , m_State(Type::Uninitialized)
    {
    }

    void Setup(EntityID Entity, RF_Mem::AutoPointerArray<RF_Type::UInt8>& Data,
        SharedAttributePool& SharedAttributes)
    {
        m_Entity = Entity;

        m_Transform = reinterpret_cast<Attribute::Transform*>(
            SharedAttributes.AttachToEntity(Attribute::Identifiers::Transform,
            sizeof(Attribute::Transform), m_Entity));
    }

    void ProcessMessages(const RF_Collect::SparseHashMap<RF_Type::UInt64, Comp::RendererMessage>* In,
        RF_Collect::Queue<Comp::RendererMessage>& Out)
    {
        switch(m_State)
        {
            case Type::Uninitialized:
            {
                RF_Draw::CommandBuffer cmdBuffer;

                auto sharedUBO = cmdBuffer.AddVariable<RF_Type::UInt32>(1);
                auto UBOData = cmdBuffer.AddVariable<void*>(1,true, "UBOData");
                cmdBuffer.State("init");
                cmdBuffer.Call<RF_Draw::GLFunctions::CreateBuffers>(1, sharedUBO.Ptr());
                cmdBuffer.Call<RF_Draw::GLFunctions::NamedBufferData>(sharedUBO,
                    sizeof(SharedTransformUniforms), 
                    UBOData, RF_GL::GL_STREAM_DRAW);
                
                cmdBuffer.State("process");
                cmdBuffer.Call<RF_Draw::GLFunctions::ClearColor>(0.5, 0.5, 0.5, 1.0);
                cmdBuffer.Call<RF_Draw::GLFunctions::Clear>(static_cast<RF_Type::Int32>(RF_GL::GL_COLOR_BUFFER_BIT || RF_GL::GL_DEPTH_BUFFER_BIT));
                cmdBuffer.Call<RF_Draw::GLFunctions::Enable>(static_cast<RF_Type::Int32>(RF_GL::GL_DEPTH_TEST));
                cmdBuffer.Call<RF_Draw::GLFunctions::BindBufferBase>(static_cast<RF_Type::Int32>(RF_GL::GL_UNIFORM_BUFFER), 2, sharedUBO);
                cmdBuffer.Call<RF_Draw::GLFunctions::NamedBufferSubData>(sharedUBO, 0, sizeof(SharedTransformUniforms), UBOData);
                cmdBuffer.Finalize();
                m_Shape.AssignByteCode(cmdBuffer.ReleaseData());
                m_Shape.MapVariable<void*>(RF_HASH("UBOData"), &m_SharedTransformUniforms);

                Comp::RendererMessage load;
                load.What = Comp::RendererMessage::Type::OpenGLCommand;
                load.Hash = RF_HASH("camera");
                load.GLMachineCommandBuffer = cmdBuffer.Data();
                m_OpenRequests.AddLast(load.Hash);
                Out.Enqueue(load);
                m_State = Type::WaitForResources;
                break;
            }
            case Type::WaitForResources:
                for(RF_Type::Int32 i = m_OpenRequests.Count() - 1; i >= 0; --i)
                {
                    if(In->ContainsKey(m_OpenRequests[i]))
                    {
                        auto it = In->Find(m_OpenRequests[i]);
                        switch(it->second.What)
                        {

                        }
                    }
                }

                if(m_OpenRequests.Count() == 0)
                {
                    m_State = Type::Running;
                }
                break;
        }
    }

    void Process(RF_Collect::Queue<Comp::RendererMessage>& Out)
    {
        m_SharedTransformUniforms.ModelView = m_Camera.GetMatrix();
        m_SharedTransformUniforms.ModelViewProjection = GetProjection() * m_SharedTransformUniforms.ModelView;

//        Comp::RendererMessage msg;
//        msg.What = Comp::RendererMessage::Type::OpenGLCommand;
//        msg.GLMachineCommandBuffer = cmdBuffer.Data().Clone();
//        Out.Enqueue(msg);
    }

    void Resize(const RF_Geo::Size2D<>& Value)
    {
        m_Projection.SetSize(Value);
    }

    const RF_Geo::Mat4f& GetProjection()
    {
        return m_Projection.GetMatrix(m_ViewType);
    }

    RF_Draw::Camera m_Camera;
protected:
    RF_Geo::ProjectionMatrix m_Projection;
private:
    EntityID m_Entity;
    RF_Geo::Viewtype::Type m_ViewType;
    Attribute::Transform* m_Transform;
    enum class Type
    {
        Uninitialized,
        WaitForResources,
        Running
    } m_State;
    // can be placed outside(rarely used)
    RF_Collect::List<RF_Type::UInt64> m_OpenRequests;

    struct SharedTransformUniforms
    {
        RF_Geo::Mat4f ModelView;
        RF_Geo::Mat4f ModelViewProjection;
    } m_SharedTransformUniforms;

    RF_Draw::NativeShape m_Shape;
};

}

#ifndef SHORTHAND_NAMESPACE_COMP
#define SHORTHAND_NAMESPACE_COMP
namespace Comp = Technology::Components;
#endif

#endif // COMPONENTS_CAMERABEHAVIOUR_HPP