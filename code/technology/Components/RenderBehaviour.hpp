#ifndef COMPONENTS_RENDERBEHAVIOUR_HPP
#define COMPONENTS_RENDERBEHAVIOUR_HPP
#if _MSC_VER > 1000
#pragma once
#endif

#include <RFECS/BaseComponent.hpp>
#include <Components/Identifiers.hpp>
#include <SharedAttributes/Identifiers.hpp>
#include <SharedAttributes/Transform.hpp>
#include <RFECS/SharedAttributePool.hpp>
#include "Components/RendererMessage.hpp"
#include <RadonFramework/Radon.hpp>
#include <RadonFramework/backend/GL/OpenGLConstants.hpp>

namespace Technology { namespace Components {

typedef RFECS::BaseComponent<Identifiers::RenderBehaviour> RenderBehaviour;

} }

namespace RadonFastEntityComponentSystem {

template<>
struct BaseComponent<Comp::Identifiers::RenderBehaviour>
{
    BaseComponent()
    :m_Entity(0)
    , m_State(Type::Uninitialized)
    {}

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
            m_State = Type::WaitForResources;
            //     GLfloat points[] = {
            //         0.0f, 0.5f, 0.0f,
            //         0.5f, -0.5f, 0.0f,
            //         -0.5f, -0.5f, 0.0f
            //     };
            //     GLfloat colours[] = {
            //         1.0f, 0.0f, 0.0f,
            //         0.0f, 1.0f, 0.0f,
            //         0.0f, 0.0f, 1.0f
            //     };
            // 
            //     GLuint vbo = 0;
            //     glGenBuffers(1, &vbo);
            //     glBindBuffer(GL_ARRAY_BUFFER, vbo);
            //     glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
            //     GLuint colours_vbo = 0;
            //     glGenBuffers(1, &colours_vbo);
            //     glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
            //     glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);
            // 
            //     vao = 0;
            //     glGenVertexArrays(1, &vao);
            //     glBindVertexArray(vao);
            //     glBindBuffer(GL_ARRAY_BUFFER, vbo);
            //     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            //     glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
            //     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            //     glEnableVertexAttribArray(0);
            //     glEnableVertexAttribArray(1);
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
                cmdBuffer.Call<RF_Draw::GLFunctions::UseProgram>(m_Shader);
                cmdBuffer.Call<RF_Draw::GLFunctions::BindVertexArray>(m_VAO);
                cmdBuffer.Call<RF_Draw::GLFunctions::DrawArrays>(RF_GL::GL_TRIANGLES, 0, 3);
                cmdBuffer.Finalize();
                m_State = Type::Running;
            }
            break;
        }
    }

    void Process(RF_Collect::Queue<Comp::RendererMessage>& Out)
    {

    }

    void RenderPass(const RF_Type::UInt64 Pass, RF_Collect::Queue<Comp::RendererMessage>& Out)
    {
        Comp::RendererMessage msg;
        msg.What = Comp::RendererMessage::Type::OpenGLCommand;
        msg.GLMachineCommandBuffer = cmdBuffer.Data().Clone();
        Out.Enqueue(msg);
    }
private:
    EntityID m_Entity;

    Attribute::Transform* m_Transform;
    enum class Type
    {
        Uninitialized,
        WaitForResources,
        Running
    } m_State;
    // can be placed outside(rarely used)
    RF_Collect::List<RF_Type::UInt64> m_OpenRequests;

    RF_Type::UInt32 m_Shader;
    RF_Type::UInt32 m_VAO;
    RF_Draw::CommandBuffer cmdBuffer;
};

}

#ifndef SHORTHAND_NAMESPACE_COMP
#define SHORTHAND_NAMESPACE_COMP
namespace Comp = Technology::Components;
#endif

#endif // COMPONENTS_RENDERBEHAVIOUR_HPP