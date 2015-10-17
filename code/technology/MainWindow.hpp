#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <RadonFramework/Drawing/Forms/Form.hpp>
#include <RadonFramework/Drawing/Canvas3D.hpp>
#include <ComponentManagers/Renderer.hpp>

namespace Technology {

class MainWindow: public RF_Form::Form
{
public:
    void Initialize(ComponentManager::Renderer* RendererManager);
protected:
    RF_Mem::AutoPointer<RF_Draw::Canvas3D> m_Canvas;
};

}

#endif