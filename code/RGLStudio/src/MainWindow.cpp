#include "precompiled.hpp"
#include "MainWindow.hpp"
#include <RadonFramework/backend/GL/glew.h>

namespace RGLStudio {

void MainWindow::Initialize()
{
    Title(RF_Type::String("RGLStudio"));
    m_Canvas = RF_Mem::AutoPointer<RF_Draw::Canvas3D>(new RF_Draw::Canvas3D(this));
}

void MainWindow::Resize(const RF_Geo::Size2D<>& Value)
{
    RF_Form::Form::Resize(Value);
    glViewport(0, 0, Value.Width, Value.Height);
}

void MainWindow::Idle()
{
    m_Canvas->Clear();
    RF_Form::Form::Idle();
    m_Canvas->SwapBuffer();
}

}