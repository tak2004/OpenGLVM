#include "Renderer.hpp"
#include <RFECS/Allocator.hpp>
#include <RFECS/SharedAttributePool.hpp>
#include "Components/Identifiers.hpp"
#include <RadonFramework/Math/Geometry/ProjectionMatrix.hpp>
#include <RadonFramework/Math/Geometry/Quaternion.hpp>
#include <RadonFramework/backend/GL/glew.h>

using namespace RadonFastEntityComponentSystem;

namespace Technology { namespace ComponentManager {

Renderer::Renderer()
:BaseComponentManager()
,m_ActiveCamera(0)
, m_PressedMoveForward(false)
, m_PressedMoveBackward(false)
, m_PressedStrafeLeft(false)
, m_PressedStrafeRight(false)
, m_PressedFlyUp(false)
, m_PressedFlyDown(false)
, m_Drag(false)
{
    m_ComponentIdentifiers.Resize(2);
    m_ComponentIdentifiers(0) = static_cast<RF_Type::UInt32>(Comp::Identifiers::RenderBehaviour);
    m_ComponentIdentifiers(1) = static_cast<RF_Type::UInt32>(Comp::Identifiers::CameraBehaviour);
}

void Renderer::Setup(Allocator& MemoryAllocator, SharedAttributePool& AttributePool)
{
    m_Allocator = &MemoryAllocator;
    m_AttributePool = &AttributePool;
    m_MessageIn.SetDeletedKey(-1);
    m_MessageIn.SetEmptyKey(0);

    m_FPS.Update();

    RF_Mem::AutoPointerArray<RF_Type::UInt8> componentData;
    this->AttachComponentOnEntity(Comp::Identifiers::RenderBehaviour, 
                                  componentData, 0, *m_AttributePool);
    this->AttachComponentOnEntity(Comp::Identifiers::CameraBehaviour, 
                                  componentData, 1, *m_AttributePool);
}

void Renderer::Process()
{    
    RF_Collect::SparseHashMap<RF_Type::UInt64, Comp::RendererMessage>* messageIn=&m_MessageIn;
    RF_Collect::Queue<Comp::RendererMessage>* messageOut = &m_MessageOut;
        
    if(m_MessageIn.Size() > 0)
    {
        RF_Algo::ForEach(m_RenderComponents, [messageIn, messageOut](RF_Collect::Array<Comp::RenderBehaviour>::EnumeratorType& Enum) {Enum->ProcessMessages(messageIn, *messageOut); });
        RF_Algo::ForEach(m_CameraComponents, [messageIn, messageOut](RF_Collect::Array<Comp::CameraBehaviour>::EnumeratorType& Enum) {Enum->ProcessMessages(messageIn, *messageOut); });
        m_MessageIn.Clear();
        // resources requests
        ProcessRequests();
    }
    
    UpdateCamera();

    RF_Type::UInt64 Pass = 0;
    RF_Algo::ForEach(m_RenderComponents, [Pass, messageOut](RF_Collect::Array<Comp::RenderBehaviour>::EnumeratorType& Enum) {Enum->RenderPass(Pass, *messageOut); });

    // render request
    ProcessRequests();

    m_Canvas->SwapBuffer();
    m_FPS.Update();
}

RF_Type::Bool Renderer::AttachComponentOnEntity(ComponentID Component,
    RF_Mem::AutoPointerArray<RF_Type::UInt8>& ComponentData, EntityID Entity,
    SharedAttributePool& SharedAttributes)
{
    switch(Component)
    {
    case Comp::Identifiers::RenderBehaviour:
            m_RenderComponents.Resize(m_RenderComponents.Count() + 1);
            m_RenderComponents(m_RenderComponents.Count() - 1).Setup(Entity, ComponentData, SharedAttributes);
            break;
    case Comp::Identifiers::CameraBehaviour:
            m_CameraComponents.Resize(m_CameraComponents.Count() + 1);
            m_CameraComponents(m_CameraComponents.Count() - 1).Setup(Entity, ComponentData, SharedAttributes);
            break;
    default:
        return false;
    }
    m_MessageIn[1].What = Comp::RendererMessage::Type::Initialize;
    return true;
}

void Renderer::SetCanvas(RF_Draw::Canvas3D* Canvas)
{
    m_Canvas = Canvas;
}

void Renderer::Resize(const RF_Geo::Size2D<>& Value)
{
    m_CameraComponents(m_ActiveCamera).Resize(Value);
    glViewport(0, 0, Value.Width, Value.Height);
}

void Renderer::ExecuteGLCommand(RF_Mem::AutoPointerArray<RF_Type::UInt8>& Buffer)
{
    RF_Type::UInt8* p = Buffer.Get();
    RF_Type::UInt8* end = p + Buffer.Size();
    RF_Draw::GLFunctions::Type id;
    RF_Draw::GLOpCode::Type opCode;
    for(; p<end;)
    {
        id = static_cast<RF_Draw::GLFunctions::Type>(*reinterpret_cast<RF_Type::UInt16*>(p));
        opCode = static_cast<RF_Draw::GLOpCode::Type>(*reinterpret_cast<RF_Type::UInt16*>(p));
        p += sizeof(RF_Type::UInt16);
        if(id >= RF_Draw::GLFunctions::MAX)
            RF_Draw::OpenGLMachine::CallOpCode(opCode, p);
        else
            RF_Draw::OpenGLMachine::CallFunction(id);
    }
}

void Renderer::ProcessRequests()
{
    Comp::RendererMessage msg;
    while(m_MessageOut.Dequeue(msg))
    {
        switch(msg.What)
        {
            case Comp::RendererMessage::Type::OpenGLCommand:
                ExecuteGLCommand(msg.GLMachineCommandBuffer);
                break;
        }
    }
}

void Renderer::UpdateCamera()
{
    RF_Type::Float32 forward = -1.0f;
    RF_Type::Float32 up = 1.0f;
    RF_Type::Float32 right = -1.0f;    

    up *= m_FPS.Delta() * 0.01f;
    right *= m_FPS.Delta()* 0.01f;
    forward *= m_FPS.Delta()* 0.01f;

    if(m_PressedMoveForward)
        m_CameraComponents(m_ActiveCamera).m_Camera.Move(forward);
    if(m_PressedMoveBackward)
        m_CameraComponents(m_ActiveCamera).m_Camera.Move(-forward);
    if(m_PressedStrafeLeft)
        m_CameraComponents(m_ActiveCamera).m_Camera.MoveHorizontal(-right);
    if (m_PressedStrafeRight)
        m_CameraComponents(m_ActiveCamera).m_Camera.MoveHorizontal(right);
    if(m_PressedFlyUp)
        m_CameraComponents(m_ActiveCamera).m_Camera.MoveVertical(up);
    if(m_PressedFlyDown)
        m_CameraComponents(m_ActiveCamera).m_Camera.MoveVertical(-up);

    m_CameraComponents(m_ActiveCamera).Process(m_MessageOut);
}

void Renderer::KeyPressed(const RF_IO::VirtualKey& VK)
{
    if(VK == RF_IO::VirtualKey::w || VK == RF_IO::VirtualKey::W)
        m_PressedMoveForward = true;
    if(VK == RF_IO::VirtualKey::s || VK == RF_IO::VirtualKey::S)        
        m_PressedMoveBackward = true;
    if(VK == RF_IO::VirtualKey::a || VK == RF_IO::VirtualKey::A)
        m_PressedStrafeLeft = true;
    if(VK == RF_IO::VirtualKey::d || VK == RF_IO::VirtualKey::D)
        m_PressedStrafeRight = true;
    if(VK == RF_IO::VirtualKey::q || VK == RF_IO::VirtualKey::Q)
        m_PressedFlyUp = true;
    if(VK == RF_IO::VirtualKey::e || VK == RF_IO::VirtualKey::E)
        m_PressedFlyDown = true;
}

void Renderer::KeyReleased(const RF_IO::VirtualKey& VK)
{
    if(VK == RF_IO::VirtualKey::w || VK == RF_IO::VirtualKey::W)
        m_PressedMoveForward = false;
    if(VK == RF_IO::VirtualKey::s || VK == RF_IO::VirtualKey::S)
        m_PressedMoveBackward = false;
    if(VK == RF_IO::VirtualKey::a || VK == RF_IO::VirtualKey::A)
        m_PressedStrafeLeft = false;
    if(VK == RF_IO::VirtualKey::d || VK == RF_IO::VirtualKey::D)
        m_PressedStrafeRight = false;
    if(VK == RF_IO::VirtualKey::q || VK == RF_IO::VirtualKey::Q)
        m_PressedFlyUp = false;
    if(VK == RF_IO::VirtualKey::e || VK == RF_IO::VirtualKey::E)
        m_PressedFlyDown = false;
}

void Renderer::MouseButtonPressed(const RF_IO::MouseEvent& Value)
{
    m_LastMousePosition = Value.CurrentPosition;
    m_Drag = true;
}

void Renderer::MouseButtonReleased(const RF_IO::MouseEvent& Value)
{
    m_Drag = false;
}

void Renderer::MouseMove(const RF_IO::MouseEvent& Value)
{
    if(m_Drag)
    {
        auto delta = m_LastMousePosition - Value.CurrentPosition;
        if(delta.X != 0 || delta.Y != 0)
        {
            m_CameraComponents(m_ActiveCamera).m_Camera.RotateX(delta.Y * 10.0f);
            m_CameraComponents(m_ActiveCamera).m_Camera.RotateY(delta.X * 10.0f);
            m_LastMousePosition = Value.CurrentPosition;
        }
    }
}

} }