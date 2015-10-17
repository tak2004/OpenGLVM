#include "precompiled.hpp"
#include "MainWindow.hpp"

namespace Technology {

void MainWindow::Initialize(ComponentManager::Renderer* renderer)
{
    Title("Technology");
    m_Canvas = RF_Mem::AutoPointer<RF_Draw::Canvas3D>(new RF_Draw::Canvas3D(this));
    OnResize += renderer->Connector<>(&ComponentManager::Renderer::Resize);
    OnKeyPress += renderer->Connector<>(&ComponentManager::Renderer::KeyPressed);
    OnKeyRelease += renderer->Connector<>(&ComponentManager::Renderer::KeyReleased);
    OnMouseButtonPressed += renderer->Connector<>(&ComponentManager::Renderer::MouseButtonPressed);
    OnMouseButtonReleased += renderer->Connector<>(&ComponentManager::Renderer::MouseButtonReleased);
    OnMouseMove += renderer->Connector<>(&ComponentManager::Renderer::MouseMove);
    renderer->SetCanvas(m_Canvas.Get());
}

}