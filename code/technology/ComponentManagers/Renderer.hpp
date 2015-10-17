#ifndef COMPONENTPOOL_RENDERER_HPP
#define COMPONENTPOOL_RENDERER_HPP
#if _MSC_VER > 1000
#pragma once
#endif

#include <RFECS/BaseComponentManager.hpp>
#include <Components/RenderBehaviour.hpp>
#include <Components/CameraBehaviour.hpp>
#include <RadonFramework/Drawing/Canvas3D.hpp>
#include <RadonFramework/Time/Framecounter.hpp>

namespace Technology { namespace ComponentManager {

class Renderer : public RFECS::BaseComponentManager, public RF_Pattern::IObserver
{
public:
    Renderer();

    void SetCanvas(RF_Draw::Canvas3D* Canvas);

    virtual void Setup(RFECS::Allocator& MemoryAllocator, RFECS::SharedAttributePool& AttributePool);

    virtual void Process();

    virtual RF_Type::Bool AttachComponentOnEntity(RFECS::ComponentID Component, 
        RF_Mem::AutoPointerArray<RF_Type::UInt8>& ComponentData, 
        RFECS::EntityID Entity, RFECS::SharedAttributePool& SharedAttributes);

    void Resize(const RF_Geo::Size2D<>& Value);
    void KeyPressed(const RF_IO::VirtualKey& VK);
    void KeyReleased(const RF_IO::VirtualKey& VK);
    void MouseButtonPressed(const RF_IO::MouseEvent& Value);
    void MouseButtonReleased(const RF_IO::MouseEvent& Value);
    void MouseMove(const RF_IO::MouseEvent& Value);
private:
    RF_Collect::Array<Comp::RenderBehaviour> m_RenderComponents;
    RF_Collect::Array<Comp::CameraBehaviour> m_CameraComponents;
    RFECS::SharedAttributePool* m_AttributePool;

    RF_Type::Size m_ActiveCamera;
    RF_Draw::Canvas3D* m_Canvas;
    RF_Time::Framecounter m_FPS;

    RF_Type::Bool m_PressedMoveForward;
    RF_Type::Bool m_PressedMoveBackward;
    RF_Type::Bool m_PressedStrafeLeft;
    RF_Type::Bool m_PressedStrafeRight;
    RF_Type::Bool m_PressedFlyUp;
    RF_Type::Bool m_PressedFlyDown;
    RF_Type::Bool m_Drag;
    RF_Geo::Point2D<> m_LastMousePosition;

    RF_Collect::SparseHashMap<RF_Type::UInt64, Comp::RendererMessage> m_MessageIn;
    RF_Collect::Queue<Comp::RendererMessage> m_MessageOut;

    void ExecuteGLCommand(RF_Mem::AutoPointerArray<RF_Type::UInt8>& Buffer);

    void ConfigureRenderTargets();
    void ProcessRequests();
    void UpdateCamera();
};

} }

#ifndef SHORTHAND_NAMESPACE_MANAGER
#define SHORTHAND_NAMESPACE_MANAGER
namespace Manager = Technology::ComponentManager;
#endif

#endif // COMPONENTPOOL_RENDERER_HPP